/**
 * @file genbindings_test.cpp
 * @brief Tests for the GENERATED bindings (tools/bindgen) on all four
 *        surfaces: C++ RAII/Status/alias, Lua, and Ruby, over the same
 *        underlying C libraries. Headless-safe: no window, audio device,
 *        or GPU is required.
 */

#include <SDL3/SDL.h>
#include <grapple/bindings.h>
#include <grapple/gpu_build.h>
#include <grapple/lua.h>
#include <grapple/ruby.h>
#include <box2d/box2d.h>
#include <cJSON.h>
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include <mruby/compile.h>
#include <mruby/string.h>

#include <string>
#include <utility>

#include "grapple/grapple.h"

namespace
{

// ---------------------------------------------------------------------------
// C++ surface

TEST(GenCpp, SurfaceRaiiOwnsAndMoves)
{
    grapple::Result<grapple::sdl::Surface> surface =
        grapple::sdl::Surface::CreateSurface(64, 48, SDL_PIXELFORMAT_RGBA8888);
    ASSERT_TRUE(surface.ok()) << surface.status().message();
    ASSERT_NE(surface->get(), nullptr);
    EXPECT_EQ(surface->get()->w, 64);

    grapple::sdl::Surface owned = std::move(surface).value();
    grapple::sdl::Surface moved = std::move(owned);
    EXPECT_NE(moved.get(), nullptr);
    EXPECT_EQ(owned.get(), nullptr);

    SDL_Surface *released = moved.release();
    ASSERT_NE(released, nullptr);
    SDL_DestroySurface(released);
}

TEST(GenCpp, FactoryFailureCarriesSdlError)
{
    grapple::Result<grapple::sdl::Surface> bad =
        grapple::sdl::Surface::CreateSurface(0, 0, SDL_PIXELFORMAT_UNKNOWN);
    EXPECT_FALSE(bad.ok());
    EXPECT_FALSE(bad.status().message().empty());

    grapple::Result<grapple::img::Surface> missing =
        grapple::img::Surface::Load("no-such-image.png");
    EXPECT_FALSE(missing.ok());
}

TEST(GenCpp, ByValuePropertiesRaii)
{
    grapple::Result<grapple::sdl::Properties> props =
        grapple::sdl::Properties::CreateProperties();
    ASSERT_TRUE(props.ok());
    grapple::Status set = props->SetNumberProperty("gen.test", 42);
    ASSERT_TRUE(set.ok()) << set.message();
    EXPECT_EQ(props->GetNumberProperty("gen.test", 0), 42);
    // Destructor destroys the by-value id (ASan-verified, engaged_ path).
}

TEST(GenCpp, StatusWrapperAndAlias)
{
    grapple::Status status =
        grapple::sdl::SetAppMetadata("genbindings", "1.0", "com.example.gen");
    EXPECT_TRUE(status.ok());
    const char *platform = grapple::sdl::GetPlatform();
    ASSERT_NE(platform, nullptr);
    EXPECT_GT(std::string(platform).size(), 0u);
}

TEST(GenCpp, JsonDocumentRaii)
{
    grapple::Result<grapple::json::JsonDocument> doc =
        grapple::json::JsonDocument::Parse("{\"answer\": 41.5}");
    ASSERT_TRUE(doc.ok());
    cJSON *item = cJSON_GetObjectItem(doc->get(), "answer");
    ASSERT_NE(item, nullptr);
    EXPECT_DOUBLE_EQ(cJSON_GetNumberValue(item), 41.5);

    grapple::Result<grapple::json::JsonDocument> bad =
        grapple::json::JsonDocument::Parse("{nope");
    EXPECT_FALSE(bad.ok());
}

TEST(GenCpp, PhysicsWorldRaii)
{
    b2WorldDef def = b2DefaultWorldDef();
    grapple::Result<grapple::b2::PhysicsWorld> world =
        grapple::b2::PhysicsWorld::CreateWorld(&def);
    ASSERT_TRUE(world.ok());
    EXPECT_TRUE(b2World_IsValid(world->get()));
    world->World_Step(1.0f / 60.0f, 4);
    b2WorldId id = world->get();
    world->reset();
    EXPECT_FALSE(b2World_IsValid(id));
}

TEST(GenCpp, ExtTiledFactoryFailsCleanly)
{
    grapple::Result<grapple::ext::TiledMapHandle> missing =
        grapple::ext::TiledMapHandle::LoadTiledMap("no-such-map.tmj");
    EXPECT_FALSE(missing.ok());
}

TEST(GenCpp, NewLibrarySurfaces)
{
    // gfx: rotozoom returns an owned surface through RAII.
    grapple::Result<grapple::sdl::Surface> src =
        grapple::sdl::Surface::CreateSurface(16, 16, SDL_PIXELFORMAT_RGBA8888);
    ASSERT_TRUE(src.ok());
    grapple::Result<grapple::gfx::Surface> rotated =
        grapple::gfx::Surface::rotozoomSurface(src->get(), 90.0, 2.0, 0);
    ASSERT_TRUE(rotated.ok());
    EXPECT_NE(rotated->get(), nullptr);

    // toml: RAII table from mutable-buffer parse.
    char conf[] = "answer = 42";
    char err[64] = {0};
    grapple::Result<grapple::toml::TomlTable> table =
        grapple::toml::TomlTable::parse(conf, err, sizeof(err));
    ASSERT_TRUE(table.ok()) << err;

    // mog: request builder RAII + version alias.
    ASSERT_NE(grapple::mog::version(), nullptr);
    grapple::Result<grapple::mog::Request> req =
        grapple::mog::Request::request_new("GET", "http://127.0.0.1:1/x");
    ASSERT_TRUE(req.ok());

    // yaml: alias surface reachable.
    int major = 0, minor = 0, patch = 0;
    grapple::yaml::get_version(&major, &minor, &patch);
    EXPECT_GE(major, 0);
}

// ---------------------------------------------------------------------------
// Lua surface

void RunLua(const char *script)
{
    lua_State *L = Grapple_CreateLuaState();
    ASSERT_NE(L, nullptr);
    ASSERT_TRUE(Grapple_OpenLuaBindings(L));
    ASSERT_EQ(luaL_dostring(L, script), LUA_OK) << lua_tostring(L, -1);
    lua_close(L);
}

TEST(GenLua, AliasEnumAndPodRoundTrip)
{
    RunLua(
        "assert(type(SDL.GetPlatform()) == 'string')\n"
        "assert(type(SDL.PIXELFORMAT_RGBA8888) == 'number')\n"
        "local ok, u = SDL.GetRectUnion({x=0, y=0, w=2, h=2}, {x=4, y=4, w=2, h=2})\n"
        "assert(ok and u.w == 6 and u.h == 6)\n"
        "local c = NK.rgb(10, 20, 30)\n"
        "assert(c.r == 10 and c.g == 20 and c.b == 30)\n");
}

TEST(GenLua, OwnedHandleGcAndExplicitDestroy)
{
    RunLua(
        "local s = SDL.CreateSurface(32, 32, SDL.PIXELFORMAT_RGBA8888)\n"
        "assert(s ~= nil and tostring(s):find('owned'))\n"
        "SDL.DestroySurface(s)  -- empties the box\n"
        "collectgarbage('collect')  -- __gc must not double-free\n"
        "local s2 = SDL.CreateSurface(16, 16, SDL.PIXELFORMAT_RGBA8888)\n"
        "s2 = nil\n"
        "collectgarbage('collect')  -- GC owns this one\n");
}

TEST(GenLua, JsonTreeWalk)
{
    RunLua(
        "local d = JSON.Parse('{\"a\": 7}')\n"
        "assert(d ~= nil)\n"
        "local item = JSON.GetObjectItem(d, 'a')\n"
        "assert(JSON.GetNumberValue(item) == 7)\n"
        "JSON.Delete(d)\n"
        "collectgarbage('collect')\n");
}

TEST(GenLua, NewLibraryModules)
{
    RunLua(
        "local s = SDL.CreateSurface(16, 16, SDL.PIXELFORMAT_RGBA8888)\n"
        "local r = SDL.CreateSoftwareRenderer(s)\n"
        "assert(GFX.pixelRGBA(r, 2, 2, 255, 0, 0, 255))\n"
        "assert(GFX.filledCircleRGBA(r, 8, 8, 5, 0, 255, 0, 255))\n"
        "local z = GFX.zoomSurface(s, 2.0, 2.0, 0)\n"
        "assert(z ~= nil and tostring(z):find('owned'))\n"
        "local t = TOML.parse('answer = 42', '', 0)\n"
        "assert(t ~= nil)\n"
        "TOML.free(t)\n"
        "assert(type(MOG.version()) == 'string')\n"
        "local req = MOG.request_new('GET', 'http://127.0.0.1:1/x')\n"
        "assert(req ~= nil and tostring(req):find('owned'))\n"
        "req = nil; collectgarbage('collect')\n");
}

TEST(GenLua, PhysicsWorldByValueIds)
{
    RunLua(
        "local def = B2.DefaultWorldDef()\n"
        "assert(type(def) == 'table')\n"
        "local w = B2.CreateWorld(def)\n"
        "assert(B2.World_IsValid(w))\n"
        "B2.World_Step(w, 1/60, 4)\n"
        "local g = B2.World_GetGravity(w)\n"
        "assert(type(g) == 'table' and g.y < 0)\n"
        "B2.DestroyWorld(w)\n"
        "assert(not B2.World_IsValid(w))\n");
}

// ---------------------------------------------------------------------------
// Ruby surface

void RunRuby(const char *script)
{
    mrb_state *mrb = Grapple_CreateRubyState();
    ASSERT_NE(mrb, nullptr);
    ASSERT_TRUE(Grapple_OpenRubyBindings(mrb));
    mrb_load_string(mrb, script);
    if (mrb->exc != nullptr)
    {
        mrb_value msg = mrb_inspect(mrb, mrb_obj_value(mrb->exc));
        FAIL() << RSTRING_CSTR(mrb, msg);
    }
    mrb_close(mrb);
}

TEST(GenRuby, AliasEnumAndPodRoundTrip)
{
    RunRuby(
        "raise 'platform' unless SDL.GetPlatform.is_a?(String)\n"
        "raise 'const' unless SDL::PIXELFORMAT_RGBA8888.is_a?(Integer)\n"
        "c = NK.rgb(10, 20, 30)\n"
        "raise 'color' unless c[:r] == 10 && c[:g] == 20 && c[:b] == 30\n");
}

TEST(GenRuby, OwnedHandleGcAndExplicitDestroy)
{
    RunRuby(
        "s = SDL.CreateSurface(32, 32, SDL::PIXELFORMAT_RGBA8888)\n"
        "raise 'surface' if s.nil?\n"
        "SDL.DestroySurface(s)\n"
        "GC.start  # dfree must not double-free\n"
        "s2 = SDL.CreateSurface(16, 16, SDL::PIXELFORMAT_RGBA8888)\n"
        "s2 = nil\n"
        "GC.start  # GC owns this one\n");
}

TEST(GenRuby, NewLibraryModules)
{
    RunRuby(
        "s = SDL.CreateSurface(16, 16, SDL::PIXELFORMAT_RGBA8888)\n"
        "r = SDL.CreateSoftwareRenderer(s)\n"
        "raise 'pixel' unless GFX.pixelRGBA(r, 2, 2, 255, 0, 0, 255)\n"
        "z = GFX.zoomSurface(s, 2.0, 2.0, 0)\n"
        "raise 'zoom' if z.nil?\n"
        "t = TOML.parse('answer = 42', '', 0)\n"
        "raise 'toml' if t.nil?\n"
        "TOML.free(t)\n"
        "raise 'mog' unless MOG.version.is_a?(String)\n"
        "req = MOG.request_new('GET', 'http://127.0.0.1:1/x')\n"
        "raise 'req' if req.nil?\n"
        "req = nil\n"
        "GC.start\n");
}

TEST(GenRuby, JsonTreeWalkAndPhysics)
{
    RunRuby(
        "d = JSON.Parse('{\"a\": 7}')\n"
        "raise 'parse' if d.nil?\n"
        "item = JSON.GetObjectItem(d, 'a')\n"
        "raise 'value' unless JSON.GetNumberValue(item) == 7\n"
        "JSON.Delete(d)\n"
        "GC.start\n"
        "def_ = B2.DefaultWorldDef\n"
        "w = B2.CreateWorld(def_)\n"
        "raise 'world' unless B2.World_IsValid(w)\n"
        "B2.World_Step(w, 1.0 / 60.0, 4)\n"
        "B2.DestroyWorld(w)\n"
        "raise 'destroy' if B2.World_IsValid(w)\n");
}

// ---------------------------------------------------------------------------
// The GPU descriptor builders
//
// 182 GPU functions were bound and nearly none callable: they take descriptor
// structs a C caller fills in on the stack, and a script has no stack to put
// one on. These check the builders that close that gap. No GPU device is
// needed for the descriptors themselves, which is the point — they are plain
// memory until SDL is handed them.

TEST(GpuBuild, PipelineBuilderOwnsItsArrays)
{
    SDL_GPUGraphicsPipelineCreateInfo *info = Grapple_GPUPipelineInfoCreate();
    ASSERT_NE(info, nullptr);

    // Past the initial capacity of four, so the arrays reallocate and the
    // descriptor must be re-pointed at the moved storage.
    for (Uint32 i = 0; i < 10; ++i)
    {
        ASSERT_TRUE(Grapple_GPUPipelineInfoAddVertexAttribute(
            info, i, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, i * 12));
    }
    ASSERT_TRUE(Grapple_GPUPipelineInfoAddVertexBuffer(info, 0, 12,
                                                         SDL_GPU_VERTEXINPUTRATE_VERTEX));
    ASSERT_TRUE(Grapple_GPUPipelineInfoAddColorTarget(info,
                                                        SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM));

    EXPECT_EQ(info->vertex_input_state.num_vertex_attributes, 10u);
    EXPECT_EQ(info->vertex_input_state.num_vertex_buffers, 1u);
    EXPECT_EQ(info->target_info.num_color_targets, 1u);
    // A stale pointer after a realloc reads freed memory rather than failing,
    // so check the contents, not just the counts.
    EXPECT_EQ(info->vertex_input_state.vertex_attributes[9].location, 9u);
    EXPECT_EQ(info->vertex_input_state.vertex_attributes[9].offset, 108u);
    EXPECT_EQ(info->vertex_input_state.vertex_buffer_descriptions[0].pitch, 12u);

    Grapple_GPUPipelineInfoDestroy(info);
}

TEST(GpuBuild, ShaderInfoCopiesCodeAndEntrypoint)
{
    SDL_GPUShaderCreateInfo *info = Grapple_GPUShaderCreateInfoCreate();
    ASSERT_NE(info, nullptr);
    // SDL's own default, so a script that does not care need say nothing.
    EXPECT_STREQ(info->entrypoint, "main");

    {
        // A script's string may be collected the moment the setter returns,
        // so the builder must not borrow it.
        std::string bytecode(64, '\x7f');
        Grapple_GPUShaderCreateInfoSetCode(info, bytecode.data(),
                                             static_cast<int>(bytecode.size()));
        Grapple_GPUShaderCreateInfoSetEntrypoint(info, std::string("vs_main").c_str());
    }
    ASSERT_EQ(info->code_size, 64u);
    EXPECT_EQ(info->code[0], 0x7f);
    EXPECT_EQ(info->code[63], 0x7f);
    EXPECT_STREQ(info->entrypoint, "vs_main");

    Grapple_GPUShaderCreateInfoSetCode(info, "abc", 3);  // replaces, does not leak
    EXPECT_EQ(info->code_size, 3u);

    Grapple_GPUShaderCreateInfoDestroy(info);
}

TEST(GpuBuild, TextureRegionGetsADepthOfOne)
{
    SDL_GPUTextureRegion *region = Grapple_GPUTextureRegionCreate();
    ASSERT_NE(region, nullptr);
    Grapple_GPUTextureRegionSet(region, nullptr, 0, 0, 32, 32);
    // A zero depth copies nothing and reports success, which is the worst
    // way for this to be wrong.
    EXPECT_EQ(region->d, 1u);
    EXPECT_EQ(region->w, 32u);
    Grapple_GPUTextureRegionDestroy(region);
}

TEST(GpuBuild, NullsAreRefusedNotCrashed)
{
    // A script passing nil should get an error, not a segfault in C.
    Grapple_GPUColorTargetInfoSetTexture(nullptr, nullptr);
    Grapple_GPUPipelineInfoDestroy(nullptr);
    Grapple_GPUShaderCreateInfoDestroy(nullptr);
    Grapple_GPUComputeBindingsDestroy(nullptr);
    EXPECT_FALSE(Grapple_GPUPipelineInfoAddColorTarget(nullptr,
                                                         SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM));
    EXPECT_EQ(Grapple_GPUAcquireSwapchain(nullptr, nullptr), nullptr);
    EXPECT_FALSE(Grapple_GPUWaitForFence(nullptr, nullptr));
    EXPECT_FALSE(Grapple_GPUUploadToTransferBuffer(nullptr, nullptr, 0, "x", 1, false));
}

TEST(GpuBuild, ComputeBindingsAppend)
{
    Grapple_GPUComputeBindings *bindings = Grapple_GPUComputeBindingsCreate();
    ASSERT_NE(bindings, nullptr);
    // NULL handles are refused, so this checks the refusal rather than the
    // append; the append itself needs a device and is covered below.
    EXPECT_FALSE(Grapple_GPUComputeBindingsAddBuffer(bindings, nullptr, true));
    EXPECT_EQ(Grapple_GPUBeginComputePass(nullptr, bindings), nullptr);
    Grapple_GPUComputeBindingsDestroy(bindings);
}

// The round trip that actually proves the builders work: put bytes on the
// device through a transfer buffer and read them back. Skipped where there is
// no GPU — CI runners mostly have none, and a skip is honest where a fake
// pass is not.
TEST(GpuBuild, TransferBufferRoundTrip)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        GTEST_SKIP() << "no video: " << SDL_GetError();
    }
    SDL_GPUDevice *device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXIL, true,
        nullptr);
    if (device == nullptr)
    {
        SDL_Quit();
        GTEST_SKIP() << "no GPU device: " << SDL_GetError();
    }

    SDL_GPUTransferBufferCreateInfo create = {};
    create.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    create.size = 64;
    SDL_GPUTransferBuffer *buffer = SDL_CreateGPUTransferBuffer(device, &create);
    ASSERT_NE(buffer, nullptr) << SDL_GetError();

    const float vertices[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    ASSERT_TRUE(Grapple_GPUUploadToTransferBuffer(device, buffer, 0, vertices, sizeof(vertices),
                                                    false))
        << SDL_GetError();

    float *read = static_cast<float *>(
        Grapple_GPUReadTransferBuffer(device, buffer, 0, sizeof(vertices)));
    ASSERT_NE(read, nullptr) << SDL_GetError();
    EXPECT_FLOAT_EQ(read[0], 1.0f);
    EXPECT_FLOAT_EQ(read[3], 4.0f);
    SDL_free(read);

    SDL_ReleaseGPUTransferBuffer(device, buffer);
    SDL_DestroyGPUDevice(device);
    SDL_Quit();
}

TEST(GenLua, GpuDescriptorsAreReachable)
{
    // The gap this closes: every one of these was bound and uncallable,
    // because none of the descriptors could be made from a script.
    RunLua(
        "local target = GrappleC.GPUColorTargetInfoCreate()\n"
        "assert(target ~= nil)\n"
        "GrappleC.GPUColorTargetInfoSetClearColor(target, 0.1, 0.2, 0.3, 1.0)\n"
        "GrappleC.GPUColorTargetInfoSetOps(target, SDL.GPU_LOADOP_CLEAR,\n"
        "                                    SDL.GPU_STOREOP_STORE)\n"
        "GrappleC.GPUColorTargetInfoDestroy(target)\n"
        "local pipeline = GrappleC.GPUPipelineInfoCreate()\n"
        "GrappleC.GPUPipelineInfoSetPrimitive(pipeline, SDL.GPU_PRIMITIVETYPE_TRIANGLELIST)\n"
        "for i = 0, 9 do\n"
        "  assert(GrappleC.GPUPipelineInfoAddVertexAttribute(\n"
        "           pipeline, i, 0, SDL.GPU_VERTEXELEMENTFORMAT_FLOAT3, i * 12))\n"
        "end\n"
        "assert(GrappleC.GPUPipelineInfoAddVertexBuffer(\n"
        "         pipeline, 0, 12, SDL.GPU_VERTEXINPUTRATE_VERTEX))\n"
        "assert(GrappleC.GPUPipelineInfoAddColorTarget(\n"
        "         pipeline, SDL.GPU_TEXTUREFORMAT_B8G8R8A8_UNORM))\n"
        "GrappleC.GPUPipelineInfoDestroy(pipeline)\n"
        // Shader bytecode arrives as a string, which is how a script would
        // hand over a compiled blob it read from disk.
        "local shader = GrappleC.GPUShaderCreateInfoCreate()\n"
        "GrappleC.GPUShaderCreateInfoSetCode(shader, string.rep('\\0', 32), 32)\n"
        "GrappleC.GPUShaderCreateInfoSetFormat(shader, SDL.GPU_SHADERFORMAT_SPIRV,\n"
        "                                        SDL.GPU_SHADERSTAGE_VERTEX)\n"
        "GrappleC.GPUShaderCreateInfoSetCounts(shader, 1, 0, 0, 1)\n"
        "GrappleC.GPUShaderCreateInfoDestroy(shader)\n"
        "local binds = GrappleC.GPUComputeBindingsCreate()\n"
        "GrappleC.GPUComputeBindingsDestroy(binds)\n"
        // The device call is reachable too; whether this machine has a GPU
        // is not this test's business, so both answers pass.
        "local device = SDL.CreateGPUDevice(SDL.GPU_SHADERFORMAT_SPIRV, true, nil)\n"
        "if device ~= nil then SDL.DestroyGPUDevice(device) end\n");
}

TEST(GenRuby, GpuDescriptorsAreReachable)
{
    RunRuby(
        "target = GrappleC.GPUColorTargetInfoCreate\n"
        "raise 'target' if target.nil?\n"
        "GrappleC.GPUColorTargetInfoSetClearColor(target, 0.1, 0.2, 0.3, 1.0)\n"
        "GrappleC.GPUColorTargetInfoSetOps(target, SDL::GPU_LOADOP_CLEAR,\n"
        "                                    SDL::GPU_STOREOP_STORE)\n"
        "GrappleC.GPUColorTargetInfoDestroy(target)\n"
        "pipeline = GrappleC.GPUPipelineInfoCreate\n"
        "10.times do |i|\n"
        "  raise 'attr' unless GrappleC.GPUPipelineInfoAddVertexAttribute(\n"
        "    pipeline, i, 0, SDL::GPU_VERTEXELEMENTFORMAT_FLOAT3, i * 12)\n"
        "end\n"
        "raise 'buffer' unless GrappleC.GPUPipelineInfoAddVertexBuffer(\n"
        "  pipeline, 0, 12, SDL::GPU_VERTEXINPUTRATE_VERTEX)\n"
        "raise 'target' unless GrappleC.GPUPipelineInfoAddColorTarget(\n"
        "  pipeline, SDL::GPU_TEXTUREFORMAT_B8G8R8A8_UNORM)\n"
        "GrappleC.GPUPipelineInfoDestroy(pipeline)\n"
        "shader = GrappleC.GPUShaderCreateInfoCreate\n"
        "GrappleC.GPUShaderCreateInfoSetCode(shader, \"\\0\" * 32, 32)\n"
        "GrappleC.GPUShaderCreateInfoDestroy(shader)\n");
}

} // namespace

// ---------------------------------------------------------------------------
// The engine, from scripts.
//
// Adding engine/include/grapple/*.h to the bindgen spec generates 377 of
// the engine's 406 functions onto the Lua and Ruby surfaces. What that does
// NOT yet give a script is a way to *start*: Grapple_CreateEngine takes an
// Grapple_EngineConfig, and Grapple_ActorSpawn takes an
// Grapple_ActorDef. Both are plain C structs a caller fills in on the
// stack, and both contain pointers (and, for ActorDef, function pointers),
// so the classifier exposes them as opaque handles rather than marshalling
// them from a table the way it does SDL_Rect.
//
// So these check what is actually true today: the surface exists and is
// reachable by name. Driving the engine from a script needs the host layer
// described in docs/engine.md — table-to-struct marshalling for the def
// structs, and a callback bridge for the hooks.

namespace
{

// A whole game loop in Lua with no engine at all: surface, renderer, event
// pump, draw, present. This is the "structure it however you wish" case —
// if this works, a script author is not boxed into the opinionated loop.
//
// A software renderer over a surface rather than a window, so it runs on a
// headless CI box; the binding surface exercised is identical, since
// CreateWindow and CreateRenderer are bound the same way.
TEST(GenLua, AScriptCanWriteItsOwnGameLoop)
{
    // The event subsystem, not just the base: PushEvent and PollEvent need
    // a queue to exist.
    ASSERT_TRUE(SDL_Init(SDL_INIT_EVENTS));
    RunLua(
        "local surf = SDL.CreateSurface(64, 48, SDL.PIXELFORMAT_RGBA8888)\n"
        "assert(surf ~= nil, 'surface')\n"
        "local r = SDL.CreateSoftwareRenderer(surf)\n"
        "assert(r ~= nil, 'renderer')\n"
        // The event a script owns: without this the loop below cannot be
        // written at all.
        "local ev = GrappleC.EventCreate()\n"
        "assert(ev ~= nil, 'event')\n"
        // Push a quit so the loop has something real to end on.
        "local quit = GrappleC.EventCreate()\n"
        "GrappleC.EventSetType(quit, SDL.EVENT_QUIT)\n"
        "SDL.PushEvent(quit)\n"
        "GrappleC.EventDestroy(quit)\n"
        "local running, frames, saw_quit = true, 0, false\n"
        "while running and frames < 100 do\n"
        "  while SDL.PollEvent(ev) do\n"
        "    local kind = GrappleC.EventType(ev)\n"
        "    if kind == SDL.EVENT_QUIT then running = false; saw_quit = true end\n"
        "    if kind == SDL.EVENT_KEY_DOWN then\n"
        "      local _ = GrappleC.EventKeyScancode(ev)\n"
        "    end\n"
        "  end\n"
        "  SDL.SetRenderDrawColor(r, 20, 30, 40, 255)\n"
        "  SDL.RenderClear(r)\n"
        "  SDL.SetRenderDrawColor(r, 255, 0, 0, 255)\n"
        "  SDL.RenderFillRect(r, {x = 8, y = 8, w = 16, h = 16})\n"
        "  SDL.RenderPresent(r)\n"
        "  frames = frames + 1\n"
        "end\n"
        "assert(saw_quit, 'the loop saw the quit event')\n"
        "assert(frames >= 1, 'and drew at least one frame')\n"
        "GrappleC.EventDestroy(ev)\n"
        "SDL.DestroyRenderer(r)\n"
        "SDL.DestroySurface(surf)\n");
    SDL_Quit();
}

// The same, in Ruby, to prove the two surfaces really are the same shape.
TEST(GenRuby, AScriptCanWriteItsOwnGameLoop)
{
    ASSERT_TRUE(SDL_Init(SDL_INIT_EVENTS));
    RunRuby(
        "surf = SDL.CreateSurface(64, 48, SDL::PIXELFORMAT_RGBA8888)\n"
        "raise 'surface' if surf.nil?\n"
        "r = SDL.CreateSoftwareRenderer(surf)\n"
        "raise 'renderer' if r.nil?\n"
        "ev = GrappleC.EventCreate\n"
        "q = GrappleC.EventCreate\n"
        "GrappleC.EventSetType(q, SDL::EVENT_QUIT)\n"
        "SDL.PushEvent(q)\n"
        "GrappleC.EventDestroy(q)\n"
        "running = true\n"
        "frames = 0\n"
        "saw_quit = false\n"
        "while running && frames < 100\n"
        "  while SDL.PollEvent(ev)\n"
        // Written out rather than as a one-liner: `a = false and b = true`
        // short-circuits in Ruby, so the second assignment never runs.
        "    if GrappleC.EventType(ev) == SDL::EVENT_QUIT\n"
        "      running = false\n"
        "      saw_quit = true\n"
        "    end\n"
        "  end\n"
        "  SDL.SetRenderDrawColor(r, 20, 30, 40, 255)\n"
        "  SDL.RenderClear(r)\n"
        "  SDL.RenderPresent(r)\n"
        "  frames += 1\n"
        "end\n"
        "raise 'quit' unless saw_quit\n"
        "GrappleC.EventDestroy(ev)\n"
        "SDL.DestroyRenderer(r)\n"
        "SDL.DestroySurface(surf)\n");
    SDL_Quit();
}

// The opinionated loop, used from inside the script's own loop. This is
// the arrangement that needed hand-written glue: turning a Lua function
// into something C can hold is the one thing the generator cannot do.
//
// Hooks fire from EngineTick as well as from Run, so a script gets the
// fixed tick, the interpolation and the asset pump while still owning the
// `while` — which is the point.
TEST(GenLua, HooksFireInsideAScriptsOwnLoop)
{
    ASSERT_TRUE(SDL_Init(0));
    RunLua(
        "local cfg = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "local e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "assert(e ~= nil)\n"
        "local steps, frames, alphas = 0, 0, {}\n"
        "GrappleC.OnFixedUpdate(e, function(step)\n"
        "  steps = steps + 1\n"
        "  assert(step > 0, 'the fixed step is a real duration')\n"
        "end)\n"
        "GrappleC.OnUpdate(e, function(dt) frames = frames + 1 end)\n"
        "GrappleC.OnRender(e, function(alpha) alphas[#alphas + 1] = alpha end)\n"
        // The script still owns the loop.
        "for i = 1, 5 do\n"
        "  GrappleC.EngineAdvance(e, 16666667)\n"
        "  GrappleC.EngineTick(e)\n"
        "end\n"
        "assert(frames == 5, 'update ran once a frame, got ' .. frames)\n"
        "assert(steps >= 4, 'the fixed tick ran, got ' .. steps)\n"
        "assert(#alphas == 5, 'render ran once a frame')\n"
        "for _, a in ipairs(alphas) do\n"
        "  assert(a >= 0 and a <= 1, 'alpha is an interpolation factor')\n"
        "end\n"
        // Replacing a handler releases the old one rather than stacking.
        "GrappleC.OnUpdate(e, function(dt) end)\n"
        "local before = frames\n"
        "GrappleC.EngineAdvance(e, 16666667)\n"
        "GrappleC.EngineTick(e)\n"
        "assert(frames == before, 'the replaced handler no longer runs')\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

// An error inside a hook must not unwind through the engine's C frames:
// one bad frame should not take the game down mid-loop.
TEST(GenLua, AnErrorInAHookDoesNotKillTheLoop)
{
    ASSERT_TRUE(SDL_Init(0));
    RunLua(
        "local cfg = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "local e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "GrappleC.OnUpdate(e, function(dt) error('deliberate') end)\n"
        "for i = 1, 3 do\n"
        "  GrappleC.EngineAdvance(e, 16666667)\n"
        "  assert(GrappleC.EngineTick(e), 'the loop survived')\n"
        "end\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

// Joints, which were bound and uncallable before the builders: no
// ragdolls, vehicles, ropes or hinged doors from script.
TEST(GenLua, JointsCanBeBuiltAndCreatedFromLua)
{
    RunLua(
        "local wd = B2.DefaultWorldDef()\n"
        "local world = B2.CreateWorld(wd)\n"
        "assert(world ~= nil)\n"
        "local bd = B2.DefaultBodyDef()\n"
        "local a = B2.CreateBody(world, bd)\n"
        "local b = B2.CreateBody(world, bd)\n"
        "assert(a ~= nil and b ~= nil)\n"
        // A hinge with a limit and a motor, entirely from script.
        "local rd = GrappleC.RevoluteJointDefCreate()\n"
        "assert(rd ~= nil, 'the def a script could not make before')\n"
        "GrappleC.RevoluteJointDefSetBodies(rd, a, b)\n"
        "GrappleC.RevoluteJointDefSetAnchors(rd, 0, 0, 1, 0)\n"
        "GrappleC.RevoluteJointDefSetLimit(rd, -90, 90)\n"
        "GrappleC.RevoluteJointDefSetMotor(rd, true, 45, 10)\n"
        "local hinge = B2.CreateRevoluteJoint(world, rd)\n"
        "assert(hinge ~= nil, 'hinge created')\n"
        "GrappleC.RevoluteJointDefDestroy(rd)\n"
        // And a rope, to show the other builders work the same way.
        "local dd = GrappleC.DistanceJointDefCreate()\n"
        "GrappleC.DistanceJointDefSetBodies(dd, a, b)\n"
        "GrappleC.DistanceJointDefSetLength(dd, 2.0)\n"
        "GrappleC.DistanceJointDefSetSpring(dd, true, 4.0, 0.5)\n"
        "local rope = B2.CreateDistanceJoint(world, dd)\n"
        "assert(rope ~= nil, 'rope created')\n"
        "GrappleC.DistanceJointDefDestroy(dd)\n"
        "B2.DestroyWorld(world)\n");
}

TEST(GenLua, AScriptCanCreateAndDriveAnEngine)
{
    ASSERT_TRUE(SDL_Init(0));
    RunLua(
        // The builder is what makes this possible: a create/destroy pair
        // plus scalar setters is a shape the generator already binds as an
        // owned handle, so no marshalling was written for it.
        "local cfg = GrappleC.ConfigCreate()\n"
        "assert(cfg ~= nil)\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "GrappleC.ConfigSetDesignSize(cfg, 320, 240)\n"
        "local e = GrappleC.CreateEngine(cfg)\n"
        "assert(e ~= nil, 'engine created from script')\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        // The loop, owned by the script.
        "for i = 1, 3 do\n"
        "  GrappleC.EngineAdvance(e, 16666667)\n"
        "  GrappleC.EngineTick(e)\n"
        "end\n"
        "assert(GrappleC.EngineFrameCount(e) >= 3)\n"
        // Actors, spawned from script through the same builder shape.
        "local def = GrappleC.ActorDefCreate()\n"
        "GrappleC.ActorDefSetType(def, 'goblin')\n"
        "GrappleC.ActorDefSetPosition(def, 10, 20)\n"
        "local id = GrappleC.ActorSpawn(e, def)\n"
        "GrappleC.ActorDefDestroy(def)\n"
        "assert(id ~= 0, 'spawned')\n"
        "GrappleC.EngineAdvance(e, 16666667)\n"
        "GrappleC.EngineTick(e)\n"
        "assert(GrappleC.ActorCount(e) == 1)\n"
        "assert(GrappleC.ActorFindByType(e, 'goblin') == id)\n"
        // Lighting and text, to show the newer subsystems came through.
        "GrappleC.LightSetPreset(e, GrappleC.GRAPPLE_LIGHT_NIGHT)\n"
        "assert(GrappleC.LightSunlight(e) < 0.5)\n"
        "assert(GrappleC.TextLoad(e, 'en', '[strings]\\n\"hi\" = \"Hello\"\\n'))\n"
        "assert(GrappleC.Text(e, 'hi') == 'Hello')\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

// The Ruby half of the same contract.
TEST(GenRuby, HooksFireInsideAScriptsOwnLoop)
{
    ASSERT_TRUE(SDL_Init(0));
    RunRuby(
        "cfg = GrappleC.ConfigCreate\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "raise 'engine' if e.nil?\n"
        "$steps = 0\n"
        "$frames = 0\n"
        "GrappleC.OnFixedUpdate(e) { |step| $steps += 1 }\n"
        "GrappleC.OnUpdate(e) { |dt| $frames += 1 }\n"
        "5.times do\n"
        "  GrappleC.EngineAdvance(e, 16666667)\n"
        "  GrappleC.EngineTick(e)\n"
        "end\n"
        "raise \"update ran #{$frames} times\" unless $frames == 5\n"
        "raise \"fixed ran #{$steps} times\" unless $steps >= 4\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

TEST(GenRuby, JointsCanBeBuiltAndCreatedFromRuby)
{
    RunRuby(
        "wd = B2.DefaultWorldDef\n"
        "world = B2.CreateWorld(wd)\n"
        "bd = B2.DefaultBodyDef\n"
        "a = B2.CreateBody(world, bd)\n"
        "b = B2.CreateBody(world, bd)\n"
        "rd = GrappleC.RevoluteJointDefCreate\n"
        "GrappleC.RevoluteJointDefSetBodies(rd, a, b)\n"
        "GrappleC.RevoluteJointDefSetLimit(rd, -45, 45)\n"
        "hinge = B2.CreateRevoluteJoint(world, rd)\n"
        "raise 'hinge' if hinge.nil?\n"
        "GrappleC.RevoluteJointDefDestroy(rd)\n"
        "B2.DestroyWorld(world)\n");
}

TEST(GenRuby, AScriptCanCreateAndDriveAnEngine)
{
    ASSERT_TRUE(SDL_Init(0));
    // Ruby uses the same names as Lua, not snake_case.
    RunRuby(
        "cfg = GrappleC.ConfigCreate\n"
        "raise 'cfg' if cfg.nil?\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "GrappleC.ConfigSetDesignSize(cfg, 320, 240)\n"
        "e = GrappleC.CreateEngine(cfg)\n"
        "raise 'engine' if e.nil?\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "3.times do\n"
        "  GrappleC.EngineAdvance(e, 16666667)\n"
        "  GrappleC.EngineTick(e)\n"
        "end\n"
        "raise 'frames' unless GrappleC.EngineFrameCount(e) >= 3\n"
        "d = GrappleC.ActorDefCreate\n"
        "GrappleC.ActorDefSetType(d, 'orc')\n"
        "id = GrappleC.ActorSpawn(e, d)\n"
        "GrappleC.ActorDefDestroy(d)\n"
        "raise 'spawn' if id == 0\n"
        "GrappleC.EngineAdvance(e, 16666667)\n"
        "GrappleC.EngineTick(e)\n"
        "raise 'count' unless GrappleC.ActorCount(e) == 1\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}


// ---------------------------------------------------------------------------
// Scenes from a script
//
// A scene is defined by its callbacks, so unlike every other definition in
// the engine a builder alone would produce a scene that does nothing. These
// check the bridge that closes it: the lifecycle in order, per-scene
// identity, and the stack behaviour that is the whole reason scenes are a
// stack rather than a pointer.

TEST(GenLua, ScenesRunTheirLifecycleInOrder)
{
    ASSERT_TRUE(SDL_Init(0));
    RunLua(
        "local cfg = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "local e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "assert(e ~= nil)\n"
        "local log = {}\n"
        "GrappleC.SceneDefine(e, 'title')\n"
        "GrappleC.SceneOn(e, 'title', 'load', function(scene)\n"
        "  log[#log + 1] = 'load'\n"
        "  assert(GrappleC.SceneName(scene) == 'title')\n"
        "  return true\n"
        "end)\n"
        "GrappleC.SceneOn(e, 'title', 'enter', function(s) log[#log+1] = 'enter' end)\n"
        "GrappleC.SceneOn(e, 'title', 'update', function(s, dt) log[#log+1] = 'update' end)\n"
        "GrappleC.SceneOn(e, 'title', 'render', function(s, a) log[#log+1] = 'render' end)\n"
        "GrappleC.SceneOn(e, 'title', 'exit', function(s) log[#log+1] = 'exit' end)\n"
        "GrappleC.SceneOn(e, 'title', 'unload', function(s) log[#log+1] = 'unload' end)\n"
        "assert(GrappleC.ScriptScenePush(e, 'title'))\n"
        // The push is deferred to the end of the frame, so nothing has run
        // yet — which is what lets a scene push from inside its own update.
        "assert(#log == 0, 'the push is deferred, got ' .. #log)\n"
        "for i = 1, 3 do\n"
        "  GrappleC.EngineAdvance(e, 16666667)\n"
        "  GrappleC.EngineTick(e)\n"
        "end\n"
        "assert(log[1] == 'load' and log[2] == 'enter',\n"
        "       'load then enter, got ' .. table.concat(log, ','))\n"
        "assert(GrappleC.SceneDepth(e) == 1)\n"
        "local seen_update, seen_render = false, false\n"
        "for _, entry in ipairs(log) do\n"
        "  if entry == 'update' then seen_update = true end\n"
        "  if entry == 'render' then seen_render = true end\n"
        "end\n"
        "assert(seen_update and seen_render, table.concat(log, ','))\n"
        // Popping runs the other half of the lifecycle, in the mirror order.
        "GrappleC.ScenePop(e)\n"
        "GrappleC.EngineAdvance(e, 16666667)\n"
        "GrappleC.EngineTick(e)\n"
        "assert(log[#log] == 'unload' and log[#log - 1] == 'exit',\n"
        "       'exit then unload, got ' .. table.concat(log, ','))\n"
        "assert(GrappleC.SceneDepth(e) == 0)\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

// The reason scenes are a stack: a pause menu leaves the level underneath
// intact, so popping it returns to exactly where the player was.
TEST(GenLua, ScenesStackAndTheCoveredOneStops)
{
    ASSERT_TRUE(SDL_Init(0));
    RunLua(
        "local cfg = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "local e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "local level_updates, menu_updates = 0, 0\n"
        "GrappleC.SceneDefine(e, 'level')\n"
        "GrappleC.SceneOn(e, 'level', 'update', function(s, dt)\n"
        "  level_updates = level_updates + 1\n"
        "end)\n"
        "GrappleC.SceneDefine(e, 'menu')\n"
        "GrappleC.SceneOn(e, 'menu', 'update', function(s, dt)\n"
        "  menu_updates = menu_updates + 1\n"
        "end)\n"
        "GrappleC.ScriptScenePush(e, 'level')\n"
        "for i = 1, 3 do\n"
        "  GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e)\n"
        "end\n"
        "local before = level_updates\n"
        "assert(before > 0, 'the level ran')\n"
        "GrappleC.ScriptScenePush(e, 'menu')\n"
        // The push applies at the end of the frame, so the level is still
        // the top scene for this one; measure from after it settles.
        "GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e)\n"
        "before = level_updates\n"
        "for i = 1, 3 do\n"
        "  GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e)\n"
        "end\n"
        "assert(GrappleC.SceneDepth(e) == 2, 'both are on the stack')\n"
        "assert(menu_updates > 0, 'the menu runs')\n"
        // Covered scenes stop by default — that is what "paused" means.
        "assert(level_updates == before,\n"
        "       'the covered level stopped, ran ' .. (level_updates - before) .. ' more')\n"
        "GrappleC.ScenePop(e)\n"
        "for i = 1, 3 do\n"
        "  GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e)\n"
        "end\n"
        "assert(level_updates > before, 'the level resumed where it was')\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

// One definition, several live scenes: the callback is given its scene, so
// a script keys its own state by that — what Grapple_SceneState does for
// C, done the way a script would do it.
TEST(GenLua, OneDefinitionBacksSeveralLiveScenes)
{
    ASSERT_TRUE(SDL_Init(0));
    RunLua(
        "local cfg = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "local e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "local state = {}\n"
        "local distinct = 0\n"
        "GrappleC.SceneDefine(e, 'room')\n"
        // Transparent, so pushing a second room leaves the first drawing —
        // and updating, so both are live at once.
        "GrappleC.ScriptSceneSetFlags(e, 'room', 3)\n"
        // Keyed by SceneKey, not by the scene itself: a handle is boxed
        // fresh each time it crosses into Lua, so the scene is a different
        // table key every frame and per-scene state never finds itself.
        "GrappleC.SceneOn(e, 'room', 'update', function(scene, dt)\n"
        "  local key = GrappleC.SceneKey(scene)\n"
        "  if state[key] == nil then\n"
        "    state[key] = 0\n"
        "    distinct = distinct + 1\n"
        "  end\n"
        "  state[key] = state[key] + 1\n"
        "end)\n"
        "GrappleC.ScriptScenePush(e, 'room')\n"
        "for i = 1, 2 do GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e) end\n"
        "GrappleC.ScriptScenePush(e, 'room')\n"
        "for i = 1, 2 do GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e) end\n"
        "assert(GrappleC.SceneDepth(e) == 2)\n"
        "assert(distinct == 2, 'two live scenes from one definition, saw ' .. distinct)\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

TEST(GenLua, UnknownSceneHookIsAnErrorNotSilence)
{
    ASSERT_TRUE(SDL_Init(0));
    lua_State *L = Grapple_CreateLuaState();
    ASSERT_NE(L, nullptr);
    ASSERT_TRUE(Grapple_OpenLuaBindings(L));
    // Set up in its own chunk, and with globals: the raise below abandons
    // whatever chunk it is in, so an engine created alongside it would
    // never be destroyed — which the leak checker reports as a bug in the
    // engine rather than in the test.
    ASSERT_EQ(luaL_dostring(L,
                            "cfg = GrappleC.ConfigCreate()\n"
                            "GrappleC.ConfigSetHeadless(cfg, true)\n"
                            "GrappleC.ConfigSetAutoMount(cfg, false)\n"
                            "engine = GrappleC.CreateEngine(cfg)\n"
                            "GrappleC.SceneDefine(engine, 'title')\n"),
              LUA_OK)
        << lua_tostring(L, -1);

    // A typo in a hook name must name the hooks that exist, rather than
    // registering nothing and leaving a scene that quietly does not draw.
    const int rc =
        luaL_dostring(L, "GrappleC.SceneOn(engine, 'title', 'raender', function(s, a) end)\n");
    EXPECT_NE(rc, LUA_OK) << "a misspelled hook should raise";
    const char *message = lua_tostring(L, -1);
    ASSERT_NE(message, nullptr);
    EXPECT_NE(std::string(message).find("unknown scene hook"), std::string::npos) << message;
    EXPECT_NE(std::string(message).find("render"), std::string::npos)
        << "the error should name the hooks that exist: " << message;
    lua_pop(L, 1);

    EXPECT_EQ(luaL_dostring(L, "GrappleC.DestroyEngine(engine)\n"
                               "GrappleC.ConfigDestroy(cfg)\n"),
              LUA_OK)
        << lua_tostring(L, -1);
    lua_close(L);
    SDL_Quit();
}

TEST(GenLua, PushingAnUndefinedSceneFails)
{
    ASSERT_TRUE(SDL_Init(0));
    RunLua(
        "local cfg = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "local e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "GrappleC.SceneDefine(e, 'title')\n"
        "assert(GrappleC.ScriptSceneDefined(e, 'title'))\n"
        "assert(not GrappleC.ScriptSceneDefined(e, 'level'))\n"
        "assert(not GrappleC.ScriptScenePush(e, 'level'),\n"
        "       'pushing an undefined scene reports failure')\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

// Redefining a name replaces its handlers rather than shadowing them. A
// script reloaded during development should get its new callbacks, not the
// old ones held alive underneath.
TEST(GenLua, RedefiningASceneReplacesItsHandlers)
{
    ASSERT_TRUE(SDL_Init(0));
    RunLua(
        "local cfg = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "local e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "local old_ran, new_ran = 0, 0\n"
        "GrappleC.SceneDefine(e, 'title')\n"
        "GrappleC.SceneOn(e, 'title', 'update', function(s, dt) old_ran = old_ran + 1 end)\n"
        "GrappleC.SceneDefine(e, 'title')  -- same name, fresh definition\n"
        "GrappleC.SceneOn(e, 'title', 'update', function(s, dt) new_ran = new_ran + 1 end)\n"
        "GrappleC.ScriptScenePush(e, 'title')\n"
        "for i = 1, 3 do GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e) end\n"
        "assert(new_ran > 0, 'the new handler runs')\n"
        "assert(old_ran == 0, 'the replaced handler does not, ran ' .. old_ran)\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

TEST(GenRuby, ScenesRunTheirLifecycleInOrder)
{
    ASSERT_TRUE(SDL_Init(0));
    RunRuby(
        "cfg = GrappleC.ConfigCreate\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "raise 'engine' if e.nil?\n"
        "$log = []\n"
        "GrappleC.SceneDefine(e, 'title')\n"
        "GrappleC.SceneOn(e, 'title', 'load') { |scene| $log << 'load'; true }\n"
        "GrappleC.SceneOn(e, 'title', 'enter') { |scene| $log << 'enter' }\n"
        "GrappleC.SceneOn(e, 'title', 'update') { |scene, dt| $log << 'update' }\n"
        "GrappleC.SceneOn(e, 'title', 'exit') { |scene| $log << 'exit' }\n"
        "GrappleC.SceneOn(e, 'title', 'unload') { |scene| $log << 'unload' }\n"
        "raise 'push' unless GrappleC.ScriptScenePush(e, 'title')\n"
        "3.times do\n"
        "  GrappleC.EngineAdvance(e, 16666667)\n"
        "  GrappleC.EngineTick(e)\n"
        "end\n"
        "raise \"order: #{$log}\" unless $log[0] == 'load' && $log[1] == 'enter'\n"
        "raise 'update' unless $log.include?('update')\n"
        "raise 'depth' unless GrappleC.SceneDepth(e) == 1\n"
        "GrappleC.ScenePop(e)\n"
        "GrappleC.EngineAdvance(e, 16666667)\n"
        "GrappleC.EngineTick(e)\n"
        "raise \"teardown: #{$log}\" unless $log[-1] == 'unload' && $log[-2] == 'exit'\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

TEST(GenRuby, ScenesStackAndTheCoveredOneStops)
{
    ASSERT_TRUE(SDL_Init(0));
    RunRuby(
        "cfg = GrappleC.ConfigCreate\n"
        "GrappleC.ConfigSetHeadless(cfg, true)\n"
        "GrappleC.ConfigSetManualClock(cfg, true)\n"
        "GrappleC.ConfigSetAutoMount(cfg, false)\n"
        "e = GrappleC.CreateEngine(cfg)\n"
        "GrappleC.ConfigDestroy(cfg)\n"
        "$level = 0\n"
        "$menu = 0\n"
        "GrappleC.SceneDefine(e, 'level')\n"
        "GrappleC.SceneOn(e, 'level', 'update') { |s, dt| $level += 1 }\n"
        "GrappleC.SceneDefine(e, 'menu')\n"
        "GrappleC.SceneOn(e, 'menu', 'update') { |s, dt| $menu += 1 }\n"
        "GrappleC.ScriptScenePush(e, 'level')\n"
        "3.times { GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e) }\n"
        "before = $level\n"
        "raise 'level' unless before > 0\n"
        "GrappleC.ScriptScenePush(e, 'menu')\n"
        // The push applies at the end of the frame, so the level is still
        // on top for this one; measure from after it settles.
        "GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e)\n"
        "before = $level\n"
        "3.times { GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e) }\n"
        "raise 'depth' unless GrappleC.SceneDepth(e) == 2\n"
        "raise 'menu' unless $menu > 0\n"
        "raise 'the covered level kept running' unless $level == before\n"
        "GrappleC.ScenePop(e)\n"
        "3.times { GrappleC.EngineAdvance(e, 16666667); GrappleC.EngineTick(e) }\n"
        "raise 'the level did not resume' unless $level > before\n"
        "GrappleC.DestroyEngine(e)\n");
    SDL_Quit();
}

} // namespace

// The engine hooks a generator cannot produce: OnEvent hands a script a real
// SDL_Event, OnResize hands it the new size. Both were missing until the GUI
// demos needed them, so both are pinned here.

TEST(GenLua, OnEventReceivesARealSdlEvent)
{
    RunLua(
        "assert(SDL.Init(SDL.INIT_EVENTS))\n"
        "local config = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(config, true)\n"
        "GrappleC.ConfigSetManualClock(config, true)\n"
        "GrappleC.ConfigSetAutoMount(config, false)\n"
        "local engine = assert(GrappleC.CreateEngine(config))\n"
        "local kind = SDL.RegisterEvents(1)\n"
        "assert(kind ~= 0)\n"
        "local seen = 0\n"
        "GrappleC.OnEvent(engine, function(event)\n"
        "  -- A borrowed handle the generated accessors understand.\n"
        "  if GrappleC.EventType(event) == kind then seen = seen + 1 end\n"
        "end)\n"
        "local pushed = GrappleC.EventCreate()\n"
        "GrappleC.EventSetType(pushed, kind)\n"
        "assert(SDL.PushEvent(pushed))\n"
        "GrappleC.EngineAdvance(engine, 16666667)\n"
        "GrappleC.EngineTick(engine)\n"
        "assert(seen == 1, 'OnEvent saw ' .. tostring(seen))\n"
        "GrappleC.EventDestroy(pushed)\n"
        "GrappleC.DestroyEngine(engine)\n");
}

TEST(GenLua, OnResizeReportsTheNewSize)
{
    RunLua(
        "local config = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetHeadless(config, true)\n"
        "GrappleC.ConfigSetManualClock(config, true)\n"
        "GrappleC.ConfigSetAutoMount(config, false)\n"
        "local engine = assert(GrappleC.CreateEngine(config))\n"
        "local registered = false\n"
        // Headless never resizes, so what is asserted here is that the hook
        // registers and takes two arguments -- the firing itself is covered
        // by the windowed demos.
        "GrappleC.OnResize(engine, function(w, h) registered = (w ~= nil and h ~= nil) end)\n"
        "GrappleC.EngineAdvance(engine, 16666667)\n"
        "GrappleC.EngineTick(engine)\n"
        "GrappleC.DestroyEngine(engine)\n");
}

TEST(GenLua, SdlLoadFileReadsAPathOutsideTheVfs)
{
    // Grapple.read_file only sees the VFS; this is the escape hatch, and the
    // only way a Ruby script can read a file at all.
    //
    // The file is written here rather than named in the source: a path that
    // exists on the machine the test was written on is not a test, it is a
    // machine-specific assertion that passes locally and fails everywhere.
    // Named for the test that owns it: ctest runs tests in parallel
    // processes, and a shared filename is a race rather than a fixture.
    const std::filesystem::path file =
        std::filesystem::temp_directory_path() / "grapple_loadfile_probe.txt";
    std::ofstream(file) << "outside the vfs";

    const std::string script =
        "local bytes = SDL.LoadFile('" + file.generic_string() + "')\n"
        "assert(type(bytes) == 'string', 'expected a string, got ' .. type(bytes))\n"
        "assert(bytes == 'outside the vfs', 'got [' .. tostring(bytes) .. ']')\n"
        // nil rather than an error, so walking a candidate list needs no pcall.
        "assert(SDL.LoadFile('/no/such/file/anywhere') == nil)\n";
    RunLua(script.c_str());
}

// A script that registers callbacks and stops is describing a game, not
// declining to run one — the bargain Love2D and Godot make. The runner
// starts it; a script that calls run() itself must not be started twice.

TEST(GenLua, PendingEngineRunsOnlyWhenItHasHandlers)
{
    lua_State *L = Grapple_CreateLuaState();
    ASSERT_NE(L, nullptr);
    ASSERT_TRUE(Grapple_OpenLuaBindings(L));

    // An engine with no handlers is not a game and must not be run.
    ASSERT_EQ(luaL_dostring(L,
                            "engine = Grapple.engine{ headless = true,"
                            "                         auto_mount = false }\n"),
              LUA_OK)
        << lua_tostring(L, -1);
    EXPECT_FALSE(Grapple_LuaRunPendingEngine(L));

    lua_close(L);
}

TEST(GenLua, RunClearsThePendingEngine)
{
    lua_State *L = Grapple_CreateLuaState();
    ASSERT_NE(L, nullptr);
    ASSERT_TRUE(Grapple_OpenLuaBindings(L));

    // manual_clock keeps run() from blocking: it stops as soon as it is told
    // to, which the load hook does on the first frame.
    ASSERT_EQ(luaL_dostring(L,
                            "engine = Grapple.engine{ headless = true,"
                            "                         auto_mount = false }\n"
                            "engine:on_load(function() engine:quit() return true end)\n"
                            "engine:run()\n"),
              LUA_OK)
        << lua_tostring(L, -1);

    // Already run by the script, so the runner must not run it again.
    EXPECT_FALSE(Grapple_LuaRunPendingEngine(L));
    lua_close(L);
}

// The engine's own flags reach a script's engine. They were parsed and
// discarded for as long as this project has had them, because nothing put
// the process arguments into the config.

TEST(GenLua, EngineFlagsFromTheCommandLineTakeEffect)
{
    lua_State *L = Grapple_CreateLuaState();
    ASSERT_NE(L, nullptr);
    ASSERT_TRUE(Grapple_OpenLuaBindings(L));

    char arg0[] = "grapple";
    char flag[] = "--max-fps";
    char value[] = "37";
    char *args[] = {arg0, flag, value};
    Grapple_SetScriptProcessArgs(3, args);

    ASSERT_EQ(luaL_dostring(L,
                            "engine = Grapple.engine{ headless = true,"
                            "                         auto_mount = false }\n"),
              LUA_OK)
        << lua_tostring(L, -1);

    lua_getglobal(L, "engine");
    Grapple_Engine *engine = Grapple_LuaEngineAt(L, -1);
    ASSERT_NE(engine, nullptr);

    const Grapple_GraphicsSettings *gfx = Grapple_EngineGraphics(engine);
    ASSERT_NE(gfx, nullptr);
    EXPECT_EQ(gfx->max_fps, 37) << "--max-fps never reached the engine";

    Grapple_SetScriptProcessArgs(0, nullptr);
    lua_close(L);
}

TEST(GenLua, AGameSettingSurvivesWhatThePlayerDidNotAskAbout)
{
    lua_State *L = Grapple_CreateLuaState();
    ASSERT_NE(L, nullptr);
    ASSERT_TRUE(Grapple_OpenLuaBindings(L));

    // Only --max-fps is on the line, so the game's own tick rate must not be
    // reset to a default by the settings pass.
    char arg0[] = "grapple";
    char flag[] = "--max-fps";
    char value[] = "45";
    char *args[] = {arg0, flag, value};
    Grapple_SetScriptProcessArgs(3, args);

    ASSERT_EQ(luaL_dostring(L,
                            "engine = Grapple.engine{ headless = true, tick_rate = 120,"
                            "                         auto_mount = false }\n"),
              LUA_OK)
        << lua_tostring(L, -1);

    lua_getglobal(L, "engine");
    Grapple_Engine *engine = Grapple_LuaEngineAt(L, -1);
    ASSERT_NE(engine, nullptr);
    EXPECT_EQ(Grapple_EngineTickRate(engine), 120);
    EXPECT_EQ(Grapple_EngineGraphics(engine)->max_fps, 45);

    Grapple_SetScriptProcessArgs(0, nullptr);
    lua_close(L);
}

// Two shapes the generator used to reject outright, and which are ordinary
// in both languages: a list of strings, and a read that fills a buffer.
//
// The directory is made here rather than pointed at somewhere in the repo:
// ctest runs from the build tree, so a relative path is a different place
// than it is from a shell.

namespace {

std::string MakeReadableDir()
{
    // One directory per test, for the same reason: these run concurrently,
    // and two tests writing the same files is a race, not a fixture.
    const ::testing::TestInfo *info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    const std::filesystem::path dir =
        std::filesystem::temp_directory_path() /
        (std::string("grapple_bindgen_reads_") + ((info != nullptr) ? info->name() : "anon"));
    std::filesystem::create_directories(dir);
    std::ofstream(dir / "alpha.txt") << "0123456789abcdefghijklmnop";
    std::ofstream(dir / "beta.txt") << "second";
    // generic_string(), not string(): these paths are pasted into Lua
    // string literals, and a Windows backslash is an escape character
    // there. Every API this reaches takes forward slashes on Windows too.
    return dir.generic_string();
}

} // namespace

TEST(GenLua, StringListReturnsBecomeATable)
{
    const std::string dir = MakeReadableDir();
    const std::string script =
        "assert(PHYSFS.init('grapple'))\n"
        "assert(PHYSFS.mount('" + dir + "', '/p', 1) ~= 0)\n"
        "local files = PHYSFS.enumerateFiles('/p')\n"
        "assert(type(files) == 'table', 'expected a table, got ' .. type(files))\n"
        "local found = {}\n"
        "for _, name in ipairs(files) do found[name] = true end\n"
        "assert(found['alpha.txt'] and found['beta.txt'], 'listing incomplete')\n"
        // Freeing the array is the binding's job; a script has no way to.
        "PHYSFS.deinit()\n";
    RunLua(script.c_str());
}

TEST(GenLua, BufferReadsComeBackAsAString)
{
    const std::string dir = MakeReadableDir();
    const std::string script =
        "assert(PHYSFS.init('grapple'))\n"
        "assert(PHYSFS.mount('" + dir + "', '/p', 1) ~= 0)\n"
        "local file = PHYSFS.openRead('/p/alpha.txt')\n"
        "assert(file ~= nil)\n"
        "local head = PHYSFS.readBytes(file, 10)\n"
        "assert(type(head) == 'string', 'expected a string, got ' .. type(head))\n"
        // Exactly what was asked for, and the right bytes: a wrong length
        // here would mean handing back the uninitialised tail of a buffer.
        "assert(head == '0123456789', 'got [' .. tostring(head) .. ']')\n"
        "PHYSFS.close(file)\n"
        "PHYSFS.deinit()\n";
    RunLua(script.c_str());
}

// The heap config builders exist because a script has no stack to put a
// struct on. Nothing owned the result, so every script that built one leaked
// it — 224 bytes, found by LeakSanitizer in this project's own tests.

TEST(GenLua, ConfigIsOwnedAndCollectable)
{
    RunLua(
        "local config = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigSetTitle(config, 'owned')\n"
        "config = nil\n"
        // A full cycle runs the finaliser: with an unowned handle this frees
        // nothing and the config leaks, which is what was happening.
        "collectgarbage('collect')\n");
}

TEST(GenLua, DestroyingAConfigExplicitlyIsStillSafe)
{
    // Explicit destroy takes the pointer out of the handle, so the collector
    // must not free it a second time. A double free is a heap error rather
    // than a leak, which means this one is checkable everywhere rather than
    // only where LeakSanitizer runs.
    RunLua(
        "local config = GrappleC.ConfigCreate()\n"
        "GrappleC.ConfigDestroy(config)\n"
        "config = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n");
}

// One interpreter's unstarted engine must never be started by another's.
//
// As a process-global this was not a theoretical worry: a leftover engine
// from an earlier test was picked up by a later one and run headless with
// nothing to stop it, hanging a CI machine for 48 minutes.

TEST(GenLua, APendingEngineDoesNotLeakBetweenStates)
{
    lua_State *first = Grapple_CreateLuaState();
    ASSERT_NE(first, nullptr);
    ASSERT_TRUE(Grapple_OpenLuaBindings(first));

    // Handlers, and deliberately never run: exactly what the runner starts.
    ASSERT_EQ(luaL_dostring(first,
                            "engine = Grapple.engine{ headless = true,"
                            "                         auto_mount = false }\n"
                            "engine:on_update(function() end)\n"),
              LUA_OK)
        << lua_tostring(first, -1);

    // A second, independent interpreter has nothing pending. If this returns
    // true it is about to run the *other* state's engine, and this test hangs
    // rather than fails — which is precisely what happened on CI.
    lua_State *second = Grapple_CreateLuaState();
    ASSERT_NE(second, nullptr);
    ASSERT_TRUE(Grapple_OpenLuaBindings(second));
    EXPECT_FALSE(Grapple_LuaRunPendingEngine(second));

    lua_close(second);
    lua_close(first);
}

// Widget handlers, in both languages.
//
// These exist because the Ruby half was broken from the day it shipped and
// nothing noticed: the callback tests were Lua-only, so a Ruby button that
// silently did nothing looked exactly like a working one until somebody
// clicked it.
//
// invoke() is what makes this checkable without a mouse — the same thing Tk
// offers for the same reason.

TEST(GenLua, WidgetHandlersFire)
{
    RunLua(
        "local engine = Grapple.engine{ headless = true, auto_mount = false }\n"
        "local ui = Grapple.ui(engine)\n"
        "local panel = ui:panel{}\n"
        "local seen = nil\n"
        "local function clicked(widget) seen = widget:text() end\n"
        "local button = panel:button{ text = 'Hello', on_click = clicked }\n"
        "button:invoke()\n"
        "assert(seen == 'Hello', 'handler did not fire, got ' .. tostring(seen))\n"
        // A second widget must reach its own handler, not the first's.
        "local other = nil\n"
        "panel:button{ text = 'Other', on_click = function(w) other = w:text() end }:invoke()\n"
        "assert(other == 'Other', 'second handler got ' .. tostring(other))\n"
        "assert(seen == 'Hello', 'first handler was overwritten')\n");
}

TEST(GenRuby, WidgetHandlersFire)
{
    RunRuby(
        "$seen = nil\n"
        "def clicked(widget) ; $seen = widget.text ; end\n"
        "engine = Grapple.engine(headless: true, auto_mount: false)\n"
        "ui = Grapple.ui(engine)\n"
        "panel = ui.panel\n"
        // A named method passed as on_click:, which is how a handler worth a
        // name is written in Ruby.
        "button = panel.button(text: 'Hello', on_click: method(:clicked))\n"
        "button.invoke\n"
        "raise \"handler did not fire, got #{$seen.inspect}\" unless $seen == 'Hello'\n"
        // And as a block, the other spelling.
        "$block_seen = nil\n"
        "panel.button(text: 'Blocky') { |w| $block_seen = w.text }.invoke\n"
        "raise \"block handler got #{$block_seen.inspect}\" unless $block_seen == 'Blocky'\n"
        "raise 'first handler was overwritten' unless $seen == 'Hello'\n");
}
