/* Original Grapple code (zlib). Expat owns XML syntax; score code owns semantics. */
#include "chip_xml.h"

#include <expat.h>

#define CHIP_XML_MAX_BYTES (64u * 1024u * 1024u)
#define CHIP_XML_MAX_NODES 300000u
#define CHIP_XML_MAX_DEPTH 128u
#define CHIP_XML_MAX_ATTRIBUTES 64u

typedef struct ChipXmlReader
{
    XML_Parser parser;
    ChipXmlNode *root;
    ChipXmlNode *current;
    size_t bytes;
    size_t nodes;
    unsigned depth;
    bool failed;
} ChipXmlReader;

static void Fail(ChipXmlReader *r, const char *message)
{
    if (!r->failed)
        SDL_SetError("MusicXML: %s at XML line %lu", message, XML_GetCurrentLineNumber(r->parser));
    r->failed = true;
    XML_StopParser(r->parser, XML_FALSE);
}

static void *Allocate(ChipXmlReader *r, size_t size)
{
    if (size > CHIP_XML_MAX_BYTES - r->bytes)
    {
        Fail(r, "document memory limit exceeded");
        return NULL;
    }
    void *result = SDL_calloc(1, size);
    if (!result)
        Fail(r, "out of memory");
    else
        r->bytes += size;
    return result;
}

static char *Copy(ChipXmlReader *r, const char *text)
{
    const size_t bytes = SDL_strlen(text) + 1;
    char *copy = Allocate(r, bytes);
    if (copy)
        SDL_memcpy(copy, text, bytes);
    return copy;
}

static const char *LocalName(const char *name)
{
    const char *separator = SDL_strrchr(name, '|');
    return separator ? separator + 1 : name;
}

static void XMLCALL Start(void *user, const XML_Char *name, const XML_Char **attributes)
{
    ChipXmlReader *r = user;
    if (r->failed)
        return;
    if (++r->depth > CHIP_XML_MAX_DEPTH || ++r->nodes > CHIP_XML_MAX_NODES)
    {
        Fail(r, "document depth or node limit exceeded");
        return;
    }
    ChipXmlNode *node = Allocate(r, sizeof(*node));
    if (!node)
        return;
    node->parent = r->current;
    if (r->current)
    {
        if (r->current->last)
            r->current->last->next = node;
        else
            r->current->children = node;
        r->current->last = node;
    }
    else
        r->root = node;
    r->current = node;
    node->line = XML_GetCurrentLineNumber(r->parser);
    node->name = Copy(r, LocalName(name));
    if (!node->name)
        return;
    size_t count = 0;
    while (attributes[count])
    {
        if (++count > CHIP_XML_MAX_ATTRIBUTES * 2)
        {
            Fail(r, "attribute limit exceeded");
            return;
        }
    }
    node->attributes = Allocate(r, (count + 1) * sizeof(char *));
    if (!node->attributes)
        return;
    for (size_t i = 0; i < count; ++i)
    {
        node->attributes[i] = Copy(r, i % 2 ? attributes[i] : LocalName(attributes[i]));
        if (!node->attributes[i])
            return;
    }
}

static void XMLCALL End(void *user, const XML_Char *name)
{
    ChipXmlReader *r = user;
    (void)name;
    if (r->failed)
        return;
    r->current = r->current->parent;
    --r->depth;
}

static void XMLCALL Text(void *user, const XML_Char *text, int length)
{
    ChipXmlReader *r = user;
    if (r->failed || !r->current || length <= 0)
        return;
    ChipXmlNode *node = r->current;
    const size_t bytes = (size_t)length;
    if (bytes + 1 > CHIP_XML_MAX_BYTES - r->bytes || node->text_size + bytes > 1024u * 1024u)
    {
        Fail(r, "text size limit exceeded");
        return;
    }
    char *larger = SDL_realloc(node->text, node->text_size + bytes + 1);
    if (!larger)
    {
        Fail(r, "out of memory");
        return;
    }
    SDL_memcpy(larger + node->text_size, text, bytes);
    node->text_size += bytes;
    larger[node->text_size] = 0;
    node->text = larger;
    r->bytes += bytes + 1;
}

static void XMLCALL Entity(void *user, const XML_Char *name, int parameter, const XML_Char *value,
                           int length, const XML_Char *base, const XML_Char *system,
                           const XML_Char *public_id, const XML_Char *notation)
{
    (void)name;
    (void)parameter;
    (void)value;
    (void)length;
    (void)base;
    (void)system;
    (void)public_id;
    (void)notation;
    Fail(user, "entity declarations are unsupported");
}

static int XMLCALL External(XML_Parser parser, const XML_Char *context, const XML_Char *base,
                            const XML_Char *system, const XML_Char *public_id)
{
    (void)context;
    (void)base;
    (void)system;
    (void)public_id;
    Fail(XML_GetUserData(parser), "external entities are unsupported");
    return XML_STATUS_ERROR;
}

ChipXmlNode *Chip_XmlParse(const void *data, size_t size)
{
    if (!data || !size || size > CHIP_XML_MAX_BYTES)
    {
        SDL_SetError("MusicXML: expected 1..64 MiB of XML input");
        return NULL;
    }
    ChipXmlReader r = {0};
    r.parser = XML_ParserCreateNS(NULL, '|');
    if (!r.parser)
    {
        SDL_SetError("MusicXML: cannot allocate XML parser");
        return NULL;
    }
    XML_SetUserData(r.parser, &r);
    XML_SetElementHandler(r.parser, Start, End);
    XML_SetCharacterDataHandler(r.parser, Text);
    XML_SetEntityDeclHandler(r.parser, Entity);
    XML_SetExternalEntityRefHandler(r.parser, External);
    const enum XML_Status status = XML_Parse(r.parser, data, (int)size, XML_TRUE);
    if (status != XML_STATUS_OK && !r.failed)
        SDL_SetError("MusicXML: %s at XML line %lu", XML_ErrorString(XML_GetErrorCode(r.parser)),
                     XML_GetCurrentLineNumber(r.parser));
    XML_ParserFree(r.parser);
    if (status != XML_STATUS_OK || r.failed)
    {
        Chip_XmlDestroy(r.root);
        return NULL;
    }
    return r.root;
}

void Chip_XmlDestroy(ChipXmlNode *node)
{
    while (node)
    {
        ChipXmlNode *next = node->next;
        Chip_XmlDestroy(node->children);
        if (node->attributes)
            for (size_t i = 0; node->attributes[i]; ++i)
                SDL_free(node->attributes[i]);
        SDL_free(node->attributes);
        SDL_free(node->name);
        SDL_free(node->text);
        SDL_free(node);
        node = next;
    }
}

const ChipXmlNode *Chip_XmlChild(const ChipXmlNode *node, const char *name)
{
    for (const ChipXmlNode *c = node ? node->children : NULL; c; c = c->next)
        if (SDL_strcmp(c->name, name) == 0)
            return c;
    return NULL;
}

const char *Chip_XmlAttribute(const ChipXmlNode *node, const char *name)
{
    if (node && node->attributes)
        for (size_t i = 0; node->attributes[i]; i += 2)
            if (SDL_strcmp(node->attributes[i], name) == 0)
                return node->attributes[i + 1];
    return "";
}

const char *Chip_XmlText(const ChipXmlNode *node, const char *name)
{
    const ChipXmlNode *c = Chip_XmlChild(node, name);
    return c && c->text ? c->text : "";
}
