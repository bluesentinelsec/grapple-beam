/*
 * grapple_engine_actor.c — the actor world.
 * Original Grapple code (zlib).
 *
 * A slot array with a generation counter per slot, a free list, and two
 * pending queues. That is the whole design, and each piece is there for a
 * specific failure it prevents:
 *
 *   Generations   make a stale handle resolve to NULL instead of to
 *                 whatever was allocated in the dead actor's place.
 *   A free list   keeps spawn O(1) without leaving holes that iteration has
 *                 to skip forever.
 *   Deferral      means the set of actors cannot change while something is
 *                 walking it, which is otherwise the most common crash in a
 *                 system like this — an actor killing another during an
 *                 update it is inside.
 *
 * The world is allocated on first spawn, so a game that never uses actors
 * pays nothing for them.
 */
#include "engine_internal.h"

#include <grapple/engine_actor.h>

/* A handle is 24 bits of index and 24 bits of generation. Keeping both in
   the low 48 bits leaves the top word free, which is what lets a script
   binding carry one through a double without losing precision. */
#define INDEX_BITS 24u
#define INDEX_MASK ((Uint64)0xFFFFFFu)
#define GENERATION_MASK ((Uint64)0xFFFFFFu)

static Grapple_ActorId MakeId(Uint32 index, Uint32 generation)
{
    /* +1 on the index so that slot 0 generation 0 is not the same bit
       pattern as GRAPPLE_ACTOR_NONE. */
    return (Grapple_ActorId)((Uint64)(index + 1u) & INDEX_MASK) |
           (((Uint64)generation & GENERATION_MASK) << INDEX_BITS);
}

static Uint32 IdIndex(Grapple_ActorId id)
{
    return (Uint32)((id & INDEX_MASK) - 1u);
}

static Uint32 IdGeneration(Grapple_ActorId id)
{
    return (Uint32)((id >> INDEX_BITS) & GENERATION_MASK);
}

struct Grapple_Actor
{
    Grapple_ActorId id;
    Grapple_Engine *engine;

    char *type;
    char *name;
    void *state;
    Grapple_ActorTags tags;

    Grapple_ActorTransform local;
    /* The transform at the last two fixed steps, so rendering can draw
       between them. Kept by the engine rather than by every actor, because
       otherwise every actor in every game reimplements it. */
    Grapple_ActorTransform previous;

    Grapple_ActorId parent;
    Grapple_ActorId *children;
    int child_count, child_capacity;

    bool (*spawn)(Grapple_Actor *actor);
    void (*fixed_update)(Grapple_Actor *actor, float step);
    void (*update)(Grapple_Actor *actor, float dt);
    void (*message)(Grapple_Actor *actor, const Grapple_ActorMessage *message);
    void (*destroy)(Grapple_Actor *actor);

    /* By value rather than allocated: a sprite is smaller than the actor
       already is, and this way it cannot be leaked or dangle. */
    Grapple_Sprite sprite;
    bool has_sprite;

    Grapple_LightDef light;
    bool has_light;

    /* A Box2D body handle, kept as its three fields so this file needs no
       Box2D header — the actor system does not depend on physics, only the
       other way round. */
    int body_index;
    Uint16 body_world, body_generation;
    float body_offset_x, body_offset_y;
    bool has_body;

    bool enabled;
    bool alive;    /* the slot holds an actor */
    bool pending;  /* spawned this frame; not yet updated or queried */
    bool doomed;   /* destroy requested; goes away at the end of the frame */
};

typedef struct QueuedMessage
{
    Grapple_ActorId target;
    Grapple_ActorMessage message;
} QueuedMessage;

struct Grapple_ActorWorld
{
    Grapple_Actor *slots;
    Uint32 *generations;
    int capacity;
    int count; /* live and not doomed */

    Uint32 *free_slots;
    int free_count;

    Grapple_ActorId *pending_spawn;
    int pending_spawn_count, pending_spawn_capacity;

    Grapple_ActorId *pending_destroy;
    int pending_destroy_count, pending_destroy_capacity;

    QueuedMessage *messages;
    int message_count, message_capacity;
    bool draining; /* messages sent now go to the next frame */
};

/* --- growth -------------------------------------------------------------- */

static bool GrowIds(Grapple_ActorId **array, int *capacity, int needed)
{
    if (*capacity >= needed)
    {
        return true;
    }
    int next = (*capacity > 0) ? *capacity * 2 : 16;
    while (next < needed)
    {
        next *= 2;
    }
    Grapple_ActorId *grown =
        (Grapple_ActorId *)SDL_realloc(*array, (size_t)next * sizeof(**array));
    if (grown == NULL)
    {
        return false;
    }
    *array = grown;
    *capacity = next;
    return true;
}

static bool GrowWorld(struct Grapple_ActorWorld *world)
{
    const int next = (world->capacity > 0) ? world->capacity * 2 : 64;
    if (next > GRAPPLE_ACTOR_MAX)
    {
        SDL_SetError("actor world is full (%d)", GRAPPLE_ACTOR_MAX);
        return false;
    }

    Grapple_Actor *slots =
        (Grapple_Actor *)SDL_realloc(world->slots, (size_t)next * sizeof(*slots));
    if (slots == NULL)
    {
        return false;
    }
    world->slots = slots;

    Uint32 *generations =
        (Uint32 *)SDL_realloc(world->generations, (size_t)next * sizeof(*generations));
    if (generations == NULL)
    {
        return false;
    }
    world->generations = generations;

    Uint32 *free_slots =
        (Uint32 *)SDL_realloc(world->free_slots, (size_t)next * sizeof(*free_slots));
    if (free_slots == NULL)
    {
        return false;
    }
    world->free_slots = free_slots;

    /* New slots start empty and free, highest index first so that the free
       list pops the lowest — which keeps the live actors bunched at the
       front of the array and iteration cache-friendly. */
    for (int i = next - 1; i >= world->capacity; --i)
    {
        SDL_zero(world->slots[i]);
        world->generations[i] = 0;
        world->free_slots[world->free_count++] = (Uint32)i;
    }
    world->capacity = next;
    return true;
}

static struct Grapple_ActorWorld *World(Grapple_Engine *engine, bool create)
{
    if (engine == NULL)
    {
        return NULL;
    }
    if (engine->actors == NULL && create)
    {
        engine->actors = (struct Grapple_ActorWorld *)SDL_calloc(
            1, sizeof(struct Grapple_ActorWorld));
    }
    return engine->actors;
}

/* --- resolving ----------------------------------------------------------- */

static Grapple_Actor *Resolve(struct Grapple_ActorWorld *world, Grapple_ActorId id,
                                bool allow_pending)
{
    if (world == NULL || id == GRAPPLE_ACTOR_NONE)
    {
        return NULL;
    }
    const Uint32 index = IdIndex(id);
    if ((int)index >= world->capacity)
    {
        return NULL;
    }
    /* The generation check is the whole point of a handle: a slot that has
       been reused has a different generation, so an old handle to it stops
       resolving rather than quietly addressing its replacement. */
    if (world->generations[index] != IdGeneration(id))
    {
        return NULL;
    }
    Grapple_Actor *actor = &world->slots[index];
    if (!actor->alive || actor->doomed)
    {
        return NULL;
    }
    if (actor->pending && !allow_pending)
    {
        return NULL;
    }
    return actor;
}

Grapple_Actor *Grapple_ActorGet(Grapple_Engine *engine, Grapple_ActorId id)
{
    /* Pending actors resolve: a spawn returns a usable handle so the caller
       can set the thing up before it starts running. */
    return Resolve(World(engine, false), id, true);
}

bool Grapple_ActorAlive(Grapple_Engine *engine, Grapple_ActorId id)
{
    return Resolve(World(engine, false), id, true) != NULL;
}

int Grapple_ActorCount(Grapple_Engine *engine)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    return (world != NULL) ? world->count : 0;
}

/* --- children ------------------------------------------------------------ */

static bool AddChild(Grapple_Actor *parent, Grapple_ActorId child)
{
    if (!GrowIds(&parent->children, &parent->child_capacity, parent->child_count + 1))
    {
        return false;
    }
    parent->children[parent->child_count++] = child;
    return true;
}

static void RemoveChild(Grapple_Actor *parent, Grapple_ActorId child)
{
    for (int i = 0; i < parent->child_count; ++i)
    {
        if (parent->children[i] == child)
        {
            /* Swap with the last: order is documented as unspecified, so
               this is free rather than a memmove. */
            parent->children[i] = parent->children[parent->child_count - 1];
            parent->child_count--;
            return;
        }
    }
}

/* --- spawn --------------------------------------------------------------- */

static char *CopyString(const char *text)
{
    return (text != NULL) ? SDL_strdup(text) : NULL;
}

Grapple_ActorId Grapple_ActorSpawn(Grapple_Engine *engine, const Grapple_ActorDef *def)
{
    struct Grapple_ActorWorld *world = World(engine, true);
    if (world == NULL || def == NULL)
    {
        SDL_InvalidParamError("engine/def");
        return GRAPPLE_ACTOR_NONE;
    }
    if (world->free_count == 0 && !GrowWorld(world))
    {
        return GRAPPLE_ACTOR_NONE;
    }

    const Uint32 index = world->free_slots[--world->free_count];
    Grapple_Actor *actor = &world->slots[index];
    SDL_zerop(actor);

    actor->id = MakeId(index, world->generations[index]);
    actor->engine = engine;
    actor->type = CopyString(def->type);
    actor->name = CopyString(def->name);
    actor->tags = def->tags;
    actor->enabled = true;
    actor->alive = true;
    actor->pending = true;

    if (def->state_size > 0)
    {
        actor->state = SDL_calloc(1, def->state_size);
        if (actor->state == NULL)
        {
            SDL_free(actor->type);
            SDL_free(actor->name);
            SDL_zerop(actor);
            world->free_slots[world->free_count++] = index;
            return GRAPPLE_ACTOR_NONE;
        }
    }

    actor->local.x = def->x;
    actor->local.y = def->y;
    actor->local.rotation = def->rotation;
    /* A zero-initialised def should produce a visible actor, not one scaled
       to nothing. */
    actor->local.scale_x = (def->scale_x != 0.0f) ? def->scale_x : 1.0f;
    actor->local.scale_y = (def->scale_y != 0.0f) ? def->scale_y : 1.0f;
    actor->previous = actor->local;

    actor->spawn = def->spawn;
    actor->fixed_update = def->fixed_update;
    actor->update = def->update;
    actor->message = def->message;
    actor->destroy = def->destroy;

    if (def->parent != GRAPPLE_ACTOR_NONE)
    {
        Grapple_Actor *parent = Resolve(world, def->parent, true);
        if (parent != NULL && AddChild(parent, actor->id))
        {
            actor->parent = def->parent;
        }
    }

    if (actor->spawn != NULL && !actor->spawn(actor))
    {
        /* The spawn hook refused. Unwind completely, including the parent
           link, so a failed spawn leaves nothing behind. */
        if (actor->parent != GRAPPLE_ACTOR_NONE)
        {
            Grapple_Actor *parent = Resolve(world, actor->parent, true);
            if (parent != NULL)
            {
                RemoveChild(parent, actor->id);
            }
        }
        SDL_free(actor->type);
        SDL_free(actor->name);
        SDL_free(actor->state);
        SDL_zerop(actor);
        world->generations[index] = (world->generations[index] + 1) & (Uint32)GENERATION_MASK;
        world->free_slots[world->free_count++] = index;
        return GRAPPLE_ACTOR_NONE;
    }

    if (!GrowIds(&world->pending_spawn, &world->pending_spawn_capacity,
                 world->pending_spawn_count + 1))
    {
        return GRAPPLE_ACTOR_NONE;
    }
    world->pending_spawn[world->pending_spawn_count++] = actor->id;
    return actor->id;
}

/* --- destroy ------------------------------------------------------------- */

/* Mark this actor and everything under it. Recursive over the hierarchy,
   which is bounded by how deeply a game nests things — a few levels, not a
   few thousand. */
static void MarkDoomed(struct Grapple_ActorWorld *world, Grapple_ActorId id)
{
    Grapple_Actor *actor = Resolve(world, id, true);
    if (actor == NULL)
    {
        return;
    }
    actor->doomed = true;
    world->count--;
    for (int i = 0; i < actor->child_count; ++i)
    {
        MarkDoomed(world, actor->children[i]);
    }
}

void Grapple_ActorDestroy(Grapple_Engine *engine, Grapple_ActorId id)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    Grapple_Actor *actor = Resolve(world, id, true);
    if (actor == NULL)
    {
        return; /* already gone: not an error, see the header */
    }
    if (!GrowIds(&world->pending_destroy, &world->pending_destroy_capacity,
                 world->pending_destroy_count + 1))
    {
        return;
    }
    world->pending_destroy[world->pending_destroy_count++] = id;
    MarkDoomed(world, id);
}

/* Free one slot, having already run its destroy hook. */
static void ReleaseSlot(struct Grapple_ActorWorld *world, Uint32 index)
{
    Grapple_Actor *actor = &world->slots[index];
    SDL_free(actor->type);
    SDL_free(actor->name);
    SDL_free(actor->state);
    SDL_free(actor->children);
    SDL_zerop(actor);

    /* Advance the generation *now*, so every handle to this actor stops
       resolving the instant the slot is free. */
    world->generations[index] = (world->generations[index] + 1) & (Uint32)GENERATION_MASK;
    world->free_slots[world->free_count++] = index;
}

/* Run destroy hooks top-down, then free bottom-up. Top-down so a parent's
   hook still sees its children; the free order does not matter once every
   hook has run. */
static void DestroyTree(struct Grapple_ActorWorld *world, Grapple_ActorId id, bool run_hooks)
{
    const Uint32 index = IdIndex(id);
    if ((int)index >= world->capacity || world->generations[index] != IdGeneration(id))
    {
        return;
    }
    Grapple_Actor *actor = &world->slots[index];
    if (!actor->alive)
    {
        return;
    }

    if (run_hooks && actor->destroy != NULL)
    {
        actor->destroy(actor);
    }

    /* Copy the children out: freeing them will edit the array. */
    const int child_count = actor->child_count;
    Grapple_ActorId *children = NULL;
    if (child_count > 0)
    {
        children = (Grapple_ActorId *)SDL_malloc((size_t)child_count * sizeof(*children));
        if (children != NULL)
        {
            SDL_memcpy(children, actor->children,
                       (size_t)child_count * sizeof(*children));
        }
    }

    if (actor->parent != GRAPPLE_ACTOR_NONE)
    {
        Grapple_Actor *parent = Resolve(world, actor->parent, true);
        if (parent == NULL)
        {
            /* The parent is being destroyed too; reach past Resolve, which
               refuses doomed actors, so the link is still cleaned up. */
            const Uint32 parent_index = IdIndex(actor->parent);
            if ((int)parent_index < world->capacity &&
                world->generations[parent_index] == IdGeneration(actor->parent) &&
                world->slots[parent_index].alive)
            {
                RemoveChild(&world->slots[parent_index], id);
            }
        }
        else
        {
            RemoveChild(parent, id);
        }
    }

    ReleaseSlot(world, index);

    for (int i = 0; i < child_count && children != NULL; ++i)
    {
        DestroyTree(world, children[i], run_hooks);
    }
    SDL_free(children);
}

void Grapple_ActorClear(Grapple_Engine *engine)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL)
    {
        return;
    }
    /* Roots only: DestroyTree takes the children with it. */
    for (int i = 0; i < world->capacity; ++i)
    {
        if (world->slots[i].alive && world->slots[i].parent == GRAPPLE_ACTOR_NONE)
        {
            DestroyTree(world, world->slots[i].id, true);
        }
    }
    /* Anything left was parented to something already gone. */
    for (int i = 0; i < world->capacity; ++i)
    {
        if (world->slots[i].alive)
        {
            DestroyTree(world, world->slots[i].id, true);
        }
    }
    world->count = 0;
    world->pending_spawn_count = 0;
    world->pending_destroy_count = 0;
    world->message_count = 0;
}

void Grapple_ActorWorldDestroy(Grapple_Engine *engine)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL)
    {
        return;
    }
    Grapple_ActorClear(engine);
    SDL_free(world->slots);
    SDL_free(world->generations);
    SDL_free(world->free_slots);
    SDL_free(world->pending_spawn);
    SDL_free(world->pending_destroy);
    SDL_free(world->messages);
    SDL_free(world);
    engine->actors = NULL;
}

/* --- sprites ------------------------------------------------------------- */

Grapple_Sprite *Grapple_ActorSpriteSlot(Grapple_Actor *actor, bool create)
{
    if (actor == NULL)
    {
        return NULL;
    }
    if (!actor->has_sprite)
    {
        if (!create)
        {
            return NULL;
        }
        actor->sprite = Grapple_SpriteDefault();
        actor->has_sprite = true;
    }
    return &actor->sprite;
}

void Grapple_ActorSpriteRemove(Grapple_Actor *actor)
{
    if (actor != NULL)
    {
        actor->has_sprite = false;
    }
}

/* --- physics bodies ------------------------------------------------------ */

void Grapple_ActorSetBody(Grapple_Actor *actor, int index, Uint16 world, Uint16 generation,
                            float offset_x, float offset_y)
{
    if (actor == NULL)
    {
        return;
    }
    actor->body_index = index;
    actor->body_world = world;
    actor->body_generation = generation;
    actor->body_offset_x = offset_x;
    actor->body_offset_y = offset_y;
    actor->has_body = true;
}

bool Grapple_ActorGetBody(Grapple_Actor *actor, int *index, Uint16 *world,
                            Uint16 *generation, float *offset_x, float *offset_y)
{
    if (actor == NULL || !actor->has_body)
    {
        return false;
    }
    if (index != NULL)
    {
        *index = actor->body_index;
    }
    if (world != NULL)
    {
        *world = actor->body_world;
    }
    if (generation != NULL)
    {
        *generation = actor->body_generation;
    }
    if (offset_x != NULL)
    {
        *offset_x = actor->body_offset_x;
    }
    if (offset_y != NULL)
    {
        *offset_y = actor->body_offset_y;
    }
    return true;
}

void Grapple_ActorClearBody(Grapple_Actor *actor)
{
    if (actor != NULL)
    {
        actor->has_body = false;
    }
}

Grapple_LightDef *Grapple_ActorLightSlot(Grapple_Actor *actor, bool create)
{
    if (actor == NULL)
    {
        return NULL;
    }
    if (!actor->has_light)
    {
        if (!create)
        {
            return NULL;
        }
        actor->light = Grapple_LightDefault();
        actor->has_light = true;
    }
    return &actor->light;
}

void Grapple_ActorLightRemove(Grapple_Actor *actor)
{
    if (actor != NULL)
    {
        actor->has_light = false;
    }
}

/* --- accessors ----------------------------------------------------------- */

Grapple_ActorId Grapple_ActorGetId(Grapple_Actor *actor)
{
    return (actor != NULL) ? actor->id : GRAPPLE_ACTOR_NONE;
}

void *Grapple_ActorState(Grapple_Actor *actor)
{
    return (actor != NULL) ? actor->state : NULL;
}

Grapple_Engine *Grapple_ActorEngine(Grapple_Actor *actor)
{
    return (actor != NULL) ? actor->engine : NULL;
}

const char *Grapple_ActorType(Grapple_Actor *actor)
{
    return (actor != NULL) ? actor->type : NULL;
}

const char *Grapple_ActorName(Grapple_Actor *actor)
{
    return (actor != NULL) ? actor->name : NULL;
}

Grapple_ActorTags Grapple_ActorGetTags(Grapple_Actor *actor)
{
    return (actor != NULL) ? actor->tags : 0;
}

void Grapple_ActorSetTags(Grapple_Actor *actor, Grapple_ActorTags tags)
{
    if (actor != NULL)
    {
        actor->tags = tags;
    }
}

bool Grapple_ActorHasTags(Grapple_Actor *actor, Grapple_ActorTags any)
{
    return (actor != NULL) && (actor->tags & any) != 0;
}

void Grapple_ActorSetEnabled(Grapple_Actor *actor, bool enabled)
{
    if (actor != NULL)
    {
        actor->enabled = enabled;
    }
}

bool Grapple_ActorEnabled(Grapple_Actor *actor)
{
    return (actor != NULL) && actor->enabled;
}

/* --- transforms ---------------------------------------------------------- */

Grapple_ActorTransform Grapple_ActorLocal(Grapple_Actor *actor)
{
    Grapple_ActorTransform identity = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f};
    return (actor != NULL) ? actor->local : identity;
}

void Grapple_ActorSetLocal(Grapple_Actor *actor, const Grapple_ActorTransform *transform)
{
    if (actor != NULL && transform != NULL)
    {
        actor->local = *transform;
    }
}

void Grapple_ActorSetPosition(Grapple_Actor *actor, float x, float y)
{
    if (actor != NULL)
    {
        actor->local.x = x;
        actor->local.y = y;
    }
}

void Grapple_ActorMove(Grapple_Actor *actor, float dx, float dy)
{
    if (actor != NULL)
    {
        actor->local.x += dx;
        actor->local.y += dy;
    }
}

void Grapple_ActorTeleport(Grapple_Actor *actor, float x, float y)
{
    if (actor == NULL)
    {
        return;
    }
    actor->local.x = x;
    actor->local.y = y;
    /* Drag the previous transform along, or the actor is drawn smeared
       across everywhere it did not go. */
    actor->previous = actor->local;
}

/* Compose child onto parent: rotate and scale the child's offset by the
   parent's transform, then translate. */
static Grapple_ActorTransform Compose(const Grapple_ActorTransform *parent,
                                        const Grapple_ActorTransform *child)
{
    const float radians = parent->rotation * (float)(SDL_PI_D / 180.0);
    const float c = SDL_cosf(radians);
    const float s = SDL_sinf(radians);
    const float x = child->x * parent->scale_x;
    const float y = child->y * parent->scale_y;

    Grapple_ActorTransform out;
    out.x = parent->x + (x * c - y * s);
    out.y = parent->y + (x * s + y * c);
    out.rotation = parent->rotation + child->rotation;
    out.scale_x = parent->scale_x * child->scale_x;
    out.scale_y = parent->scale_y * child->scale_y;
    return out;
}

/* Walk to the root composing transforms. Depth is bounded by SetParent's
   cycle check, so this terminates. */
static Grapple_ActorTransform WorldOf(struct Grapple_ActorWorld *world,
                                        Grapple_Actor *actor, bool interpolated, float alpha)
{
    Grapple_ActorTransform local = actor->local;
    if (interpolated)
    {
        local.x = actor->previous.x + (actor->local.x - actor->previous.x) * alpha;
        local.y = actor->previous.y + (actor->local.y - actor->previous.y) * alpha;
        local.rotation = actor->previous.rotation +
                         (actor->local.rotation - actor->previous.rotation) * alpha;
        local.scale_x = actor->previous.scale_x +
                        (actor->local.scale_x - actor->previous.scale_x) * alpha;
        local.scale_y = actor->previous.scale_y +
                        (actor->local.scale_y - actor->previous.scale_y) * alpha;
    }

    if (actor->parent == GRAPPLE_ACTOR_NONE)
    {
        return local;
    }
    Grapple_Actor *parent = Resolve(world, actor->parent, true);
    if (parent == NULL)
    {
        return local;
    }
    const Grapple_ActorTransform parent_world =
        WorldOf(world, parent, interpolated, alpha);
    return Compose(&parent_world, &local);
}

Grapple_ActorTransform Grapple_ActorWorld(Grapple_Actor *actor)
{
    Grapple_ActorTransform identity = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f};
    if (actor == NULL)
    {
        return identity;
    }
    return WorldOf(World(actor->engine, false), actor, false, 0.0f);
}

Grapple_ActorTransform Grapple_ActorRenderTransform(Grapple_Actor *actor, float alpha)
{
    Grapple_ActorTransform identity = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f};
    if (actor == NULL)
    {
        return identity;
    }
    return WorldOf(World(actor->engine, false), actor, true, alpha);
}

/* --- hierarchy ----------------------------------------------------------- */

Grapple_ActorId Grapple_ActorParent(Grapple_Actor *actor)
{
    return (actor != NULL) ? actor->parent : GRAPPLE_ACTOR_NONE;
}

int Grapple_ActorChildCount(Grapple_Actor *actor)
{
    return (actor != NULL) ? actor->child_count : 0;
}

Grapple_ActorId Grapple_ActorChild(Grapple_Actor *actor, int index)
{
    if (actor == NULL || index < 0 || index >= actor->child_count)
    {
        return GRAPPLE_ACTOR_NONE;
    }
    return actor->children[index];
}

/* Would making `candidate` the parent of `actor` create a cycle? */
static bool WouldCycle(struct Grapple_ActorWorld *world, Grapple_Actor *actor,
                       Grapple_ActorId candidate)
{
    Grapple_ActorId walk = candidate;
    /* Bounded by the number of slots: even a corrupt chain terminates. */
    for (int guard = 0; guard <= world->capacity && walk != GRAPPLE_ACTOR_NONE; ++guard)
    {
        if (walk == actor->id)
        {
            return true;
        }
        Grapple_Actor *step = Resolve(world, walk, true);
        if (step == NULL)
        {
            return false;
        }
        walk = step->parent;
    }
    return false;
}

/* Solve for the local transform that leaves the world transform unchanged
   under a new parent. */
static Grapple_ActorTransform Decompose(const Grapple_ActorTransform *parent_world,
                                          const Grapple_ActorTransform *world)
{
    const float radians = -parent_world->rotation * (float)(SDL_PI_D / 180.0);
    const float c = SDL_cosf(radians);
    const float s = SDL_sinf(radians);
    const float dx = world->x - parent_world->x;
    const float dy = world->y - parent_world->y;
    const float sx = (parent_world->scale_x != 0.0f) ? parent_world->scale_x : 1.0f;
    const float sy = (parent_world->scale_y != 0.0f) ? parent_world->scale_y : 1.0f;

    Grapple_ActorTransform out;
    out.x = (dx * c - dy * s) / sx;
    out.y = (dx * s + dy * c) / sy;
    out.rotation = world->rotation - parent_world->rotation;
    out.scale_x = world->scale_x / sx;
    out.scale_y = world->scale_y / sy;
    return out;
}

bool Grapple_ActorSetParent(Grapple_Actor *actor, Grapple_ActorId parent)
{
    if (actor == NULL)
    {
        SDL_InvalidParamError("actor");
        return false;
    }
    struct Grapple_ActorWorld *world = World(actor->engine, false);
    if (world == NULL || parent == actor->id)
    {
        SDL_SetError("an actor cannot be its own parent");
        return false;
    }

    Grapple_Actor *next = NULL;
    if (parent != GRAPPLE_ACTOR_NONE)
    {
        next = Resolve(world, parent, true);
        if (next == NULL)
        {
            SDL_SetError("no such parent");
            return false;
        }
        /* A cycle would make every transform walk run forever, so it is
           refused rather than detected later. */
        if (WouldCycle(world, actor, parent))
        {
            SDL_SetError("reparenting there would make a cycle");
            return false;
        }
    }

    /* Keep the world position: "pick this up" should not teleport it. */
    const Grapple_ActorTransform world_transform = Grapple_ActorWorld(actor);

    if (actor->parent != GRAPPLE_ACTOR_NONE)
    {
        Grapple_Actor *old = Resolve(world, actor->parent, true);
        if (old != NULL)
        {
            RemoveChild(old, actor->id);
        }
    }

    actor->parent = GRAPPLE_ACTOR_NONE;
    if (next != NULL)
    {
        if (!AddChild(next, actor->id))
        {
            return false;
        }
        actor->parent = parent;
        const Grapple_ActorTransform parent_world = Grapple_ActorWorld(next);
        actor->local = Decompose(&parent_world, &world_transform);
    }
    else
    {
        actor->local = world_transform;
    }
    actor->previous = actor->local;
    return true;
}

/* --- queries ------------------------------------------------------------- */

/* Does this actor match a (type, tags) filter? NULL/0 mean "any". */
static bool Matches(const Grapple_Actor *actor, const char *type, Grapple_ActorTags tags)
{
    if (type != NULL && (actor->type == NULL || SDL_strcmp(actor->type, type) != 0))
    {
        return false;
    }
    if (tags != 0 && (actor->tags & tags) == 0)
    {
        return false;
    }
    return true;
}

Grapple_ActorId Grapple_ActorFindByType(Grapple_Engine *engine, const char *type)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL || type == NULL)
    {
        return GRAPPLE_ACTOR_NONE;
    }
    for (int i = 0; i < world->capacity; ++i)
    {
        Grapple_Actor *actor = &world->slots[i];
        if (actor->alive && !actor->doomed && !actor->pending && Matches(actor, type, 0))
        {
            return actor->id;
        }
    }
    return GRAPPLE_ACTOR_NONE;
}

Grapple_ActorId Grapple_ActorFindByName(Grapple_Engine *engine, const char *name)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL || name == NULL)
    {
        return GRAPPLE_ACTOR_NONE;
    }
    for (int i = 0; i < world->capacity; ++i)
    {
        Grapple_Actor *actor = &world->slots[i];
        if (actor->alive && !actor->doomed && !actor->pending && actor->name != NULL &&
            SDL_strcmp(actor->name, name) == 0)
        {
            return actor->id;
        }
    }
    return GRAPPLE_ACTOR_NONE;
}

int Grapple_ActorQuery(Grapple_Engine *engine, const char *type, Grapple_ActorTags tags,
                         Grapple_ActorId *out, int capacity)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL || out == NULL || capacity <= 0)
    {
        return 0;
    }
    int found = 0;
    for (int i = 0; i < world->capacity && found < capacity; ++i)
    {
        Grapple_Actor *actor = &world->slots[i];
        if (actor->alive && !actor->doomed && !actor->pending && Matches(actor, type, tags))
        {
            out[found++] = actor->id;
        }
    }
    return found;
}

void Grapple_ActorEach(Grapple_Engine *engine,
                         bool (*visit)(Grapple_Actor *actor, void *user), void *user)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL || visit == NULL)
    {
        return;
    }
    for (int i = 0; i < world->capacity; ++i)
    {
        Grapple_Actor *actor = &world->slots[i];
        if (actor->alive && !actor->doomed && !actor->pending && !visit(actor, user))
        {
            return;
        }
    }
}

/* --- messages ------------------------------------------------------------ */

static bool Queue(struct Grapple_ActorWorld *world, Grapple_ActorId target,
                  const Grapple_ActorMessage *message)
{
    if (world->message_count >= world->message_capacity)
    {
        const int next = (world->message_capacity > 0) ? world->message_capacity * 2 : 32;
        QueuedMessage *grown =
            (QueuedMessage *)SDL_realloc(world->messages, (size_t)next * sizeof(*grown));
        if (grown == NULL)
        {
            return false;
        }
        world->messages = grown;
        world->message_capacity = next;
    }
    world->messages[world->message_count].target = target;
    world->messages[world->message_count].message = *message;
    world->message_count++;
    return true;
}

bool Grapple_ActorSend(Grapple_Engine *engine, Grapple_ActorId target,
                         const Grapple_ActorMessage *message)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL || message == NULL || Resolve(world, target, true) == NULL)
    {
        return false;
    }
    return Queue(world, target, message);
}

int Grapple_ActorBroadcast(Grapple_Engine *engine, const char *type,
                             Grapple_ActorTags tags, const Grapple_ActorMessage *message)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL || message == NULL)
    {
        return 0;
    }
    int sent = 0;
    for (int i = 0; i < world->capacity; ++i)
    {
        Grapple_Actor *actor = &world->slots[i];
        if (actor->alive && !actor->doomed && !actor->pending && Matches(actor, type, tags) &&
            Queue(world, actor->id, message))
        {
            sent++;
        }
    }
    return sent;
}

/* --- the frame ----------------------------------------------------------- */

void Grapple_ActorDispatchFixedUpdate(Grapple_Engine *engine, float step)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL)
    {
        return;
    }
    /* Snapshot every transform before anything moves. Doing it here, once,
       is what lets rendering interpolate without every actor keeping its
       own copy — and it has to happen before the first update of the step, or
       an actor that moves early would be interpolated from where it already
       is. */
    for (int i = 0; i < world->capacity; ++i)
    {
        Grapple_Actor *actor = &world->slots[i];
        if (actor->alive && !actor->doomed && !actor->pending)
        {
            actor->previous = actor->local;
        }
    }
    for (int i = 0; i < world->capacity; ++i)
    {
        Grapple_Actor *actor = &world->slots[i];
        if (actor->alive && !actor->doomed && !actor->pending && actor->enabled &&
            actor->fixed_update != NULL)
        {
            actor->fixed_update(actor, step);
        }
    }
}

void Grapple_ActorDispatchUpdate(Grapple_Engine *engine, float dt)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL)
    {
        return;
    }
    for (int i = 0; i < world->capacity; ++i)
    {
        Grapple_Actor *actor = &world->slots[i];
        if (actor->alive && !actor->doomed && !actor->pending && actor->enabled &&
            actor->update != NULL)
        {
            actor->update(actor, dt);
        }
    }
}

void Grapple_ActorDeliverMessages(Grapple_Engine *engine)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL || world->message_count == 0 || world->draining)
    {
        return;
    }

    /* Take the queue and drain the copy. Messages sent while draining land
       in the fresh queue and are delivered next frame, which bounds the
       work in a frame — an actor pair that answers each other forever
       becomes visibly slow rather than a stack overflow. */
    const int count = world->message_count;
    QueuedMessage *batch = (QueuedMessage *)SDL_malloc((size_t)count * sizeof(*batch));
    if (batch == NULL)
    {
        return;
    }
    SDL_memcpy(batch, world->messages, (size_t)count * sizeof(*batch));
    world->message_count = 0;

    world->draining = true;
    for (int i = 0; i < count; ++i)
    {
        Grapple_Actor *actor = Resolve(world, batch[i].target, false);
        /* A target that died earlier in this same drain simply does not
           receive it, which is what "it is gone" should mean. */
        if (actor != NULL && actor->message != NULL)
        {
            actor->message(actor, &batch[i].message);
        }
    }
    world->draining = false;
    SDL_free(batch);
}

void Grapple_ActorApplyPending(Grapple_Engine *engine)
{
    struct Grapple_ActorWorld *world = World(engine, false);
    if (world == NULL)
    {
        return;
    }

    /* Destroy first, then admit the new. A spawn made *by* a destroy hook
       must not be swept away by the same pass, and doing it in this order
       means it is not. */
    for (int i = 0; i < world->pending_destroy_count; ++i)
    {
        DestroyTree(world, world->pending_destroy[i], true);
    }
    world->pending_destroy_count = 0;

    for (int i = 0; i < world->pending_spawn_count; ++i)
    {
        Grapple_Actor *actor = Resolve(world, world->pending_spawn[i], true);
        if (actor != NULL && actor->pending)
        {
            actor->pending = false;
            world->count++;
        }
    }
    world->pending_spawn_count = 0;
}
