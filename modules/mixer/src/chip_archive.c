/* Original Grapple code (zlib). Bounded ZIP central-directory reader for MXL. */
#include "chip_archive.h"

#include "chip_xml.h"

#define ZIP_MAX_ENTRY (64u * 1024u * 1024u)
#define ZIP_MAX_TOTAL (256u * 1024u * 1024u)

typedef struct ZipEntry
{
    char *name;
    Uint32 offset, compressed, size, crc;
    Uint16 flags, method;
} ZipEntry;

static Uint16 Read16(const Uint8 *p)
{
    return (Uint16)(p[0] | (Uint16)p[1] << 8);
}
static Uint32 Read32(const Uint8 *p)
{
    return (Uint32)Read16(p) | (Uint32)Read16(p + 2) << 16;
}

static bool RelativePath(const char *name)
{
    if (!*name || *name == '/' || SDL_strchr(name, '\\') || SDL_strchr(name, ':'))
        return false;
    for (const char *part = name; *part;)
    {
        const char *end = SDL_strchr(part, '/');
        const size_t n = end ? (size_t)(end - part) : SDL_strlen(part);
        if (!n || (n == 1 && part[0] == '.') || (n == 2 && part[0] == '.' && part[1] == '.'))
            return false;
        if (!end)
            break;
        part = end + 1;
    }
    return true;
}

static void *Extract(const Uint8 *data, size_t directory, const ZipEntry *entry)
{
    const size_t local = entry->offset;
    if (local > directory || directory - local < 30 || Read32(data + local) != 0x04034b50u)
    {
        SDL_SetError("MusicXML: invalid MXL local header");
        return NULL;
    }
    const Uint8 *p = data + local;
    const size_t namesize = Read16(p + 26), extra = Read16(p + 28);
    if (Read16(p + 6) != entry->flags || Read16(p + 8) != entry->method ||
        namesize != SDL_strlen(entry->name) || namesize + extra > directory - local - 30 ||
        SDL_memcmp(p + 30, entry->name, namesize) != 0)
    {
        SDL_SetError("MusicXML: inconsistent MXL local header");
        return NULL;
    }
    const size_t start = local + 30 + namesize + extra;
    if (entry->compressed > directory - start ||
        (!(entry->flags & 8) &&
         (Read32(p + 14) != entry->crc || Read32(p + 18) != entry->compressed ||
          Read32(p + 22) != entry->size)))
    {
        SDL_SetError("MusicXML: truncated or inconsistent MXL entry");
        return NULL;
    }
    void *result = SDL_malloc((size_t)entry->size + 1);
    if (!result)
        return NULL;
    bool ok = true;
    if (entry->method == 0)
    {
        ok = entry->size == entry->compressed;
        if (ok)
            SDL_memcpy(result, data + start, entry->size);
        else
            SDL_SetError("MusicXML: inconsistent stored MXL entry sizes");
    }
    else
        ok = Chip_Inflate(data + start, entry->compressed, result, entry->size);
    if (ok && SDL_crc32(0, result, entry->size) != entry->crc)
        ok = SDL_SetError("MusicXML: MXL entry CRC mismatch");
    if (!ok)
    {
        SDL_free(result);
        return NULL;
    }
    ((Uint8 *)result)[entry->size] = 0;
    return result;
}

void *Chip_ReadMxl(const void *input, size_t size, size_t *output_size)
{
    const Uint8 *data = input;
    if (size < 22 || size > ZIP_MAX_ENTRY)
    {
        SDL_SetError("MusicXML: invalid MXL size");
        return NULL;
    }
    size_t end = size - 22;
    const size_t lower = end > 65535 ? end - 65535 : 0;
    while (Read32(data + end) != 0x06054b50u || Read16(data + end + 20) != size - end - 22)
    {
        if (end == lower)
        {
            SDL_SetError("MusicXML: missing MXL central directory");
            return NULL;
        }
        --end;
    }
    const Uint8 *eocd = data + end;
    const size_t count = Read16(eocd + 10), directory = Read32(eocd + 16),
                 bytes = Read32(eocd + 12);
    if (!count || count > 4096 || Read16(eocd + 4) || Read16(eocd + 6) ||
        Read16(eocd + 8) != count || directory > end || bytes != end - directory)
    {
        SDL_SetError("MusicXML: unsupported MXL directory (split/ZIP64 or size limit)");
        return NULL;
    }
    ZipEntry *entries = SDL_calloc(count, sizeof(*entries));
    if (!entries)
        return NULL;
    void *result = NULL;
    ChipXmlNode *container = NULL;
    size_t cursor = directory, total = 0;
    for (size_t i = 0; i < count; ++i)
    {
        if (cursor > end || end - cursor < 46 || Read32(data + cursor) != 0x02014b50u)
        {
            SDL_SetError("MusicXML: malformed MXL directory entry");
            goto done;
        }
        const Uint8 *p = data + cursor;
        const size_t namesize = Read16(p + 28), extra = Read16(p + 30), comment = Read16(p + 32);
        if (!namesize || namesize + extra + comment > end - cursor - 46 || Read16(p + 34))
        {
            SDL_SetError("MusicXML: invalid MXL entry name/size/disk");
            goto done;
        }
        ZipEntry *entry = &entries[i];
        entry->name = SDL_malloc(namesize + 1);
        if (!entry->name)
            goto done;
        SDL_memcpy(entry->name, p + 46, namesize);
        entry->name[namesize] = 0;
        if (SDL_strlen(entry->name) != namesize || !RelativePath(entry->name))
        {
            SDL_SetError("MusicXML: unsafe MXL entry path");
            goto done;
        }
        for (size_t j = 0; j < i; ++j)
            if (SDL_strcmp(entry->name, entries[j].name) == 0)
            {
                SDL_SetError("MusicXML: duplicate MXL entry path");
                goto done;
            }
        entry->flags = Read16(p + 8);
        entry->method = Read16(p + 10);
        entry->crc = Read32(p + 16);
        entry->compressed = Read32(p + 20);
        entry->size = Read32(p + 24);
        entry->offset = Read32(p + 42);
        if ((entry->flags & ~0x080eu) || (entry->method != 0 && entry->method != 8) ||
            entry->size > ZIP_MAX_ENTRY || entry->compressed > ZIP_MAX_ENTRY ||
            entry->size > ZIP_MAX_TOTAL - total)
        {
            SDL_SetError("MusicXML: unsupported MXL encryption/compression or expansion limit");
            goto done;
        }
        total += entry->size;
        cursor += 46 + namesize + extra + comment;
    }
    if (cursor != end)
    {
        SDL_SetError("MusicXML: inconsistent MXL directory size");
        goto done;
    }
    for (size_t i = 0; i < count; ++i)
        if (SDL_strcmp(entries[i].name, "META-INF/container.xml") == 0)
        {
            void *xml = Extract(data, directory, &entries[i]);
            if (!xml)
                goto done;
            container = Chip_XmlParse(xml, entries[i].size);
            SDL_free(xml);
            break;
        }
    if (!container)
    {
        SDL_SetError("MusicXML: missing or invalid MXL container.xml");
        goto done;
    }
    if (SDL_strcmp(container->name, "container"))
    {
        SDL_SetError("MusicXML: invalid container document root");
        goto done;
    }
    const ChipXmlNode *roots = Chip_XmlChild(container, "rootfiles");
    const char *path = NULL;
    for (const ChipXmlNode *root = roots ? roots->children : NULL; root; root = root->next)
        if (SDL_strcmp(root->name, "rootfile") == 0)
        {
            const char *type = Chip_XmlAttribute(root, "media-type");
            if (*type && SDL_strcmp(type, "application/vnd.recordare.musicxml+xml"))
                continue;
            if (path)
            {
                SDL_SetError("MusicXML: multiple MXL scores require an explicit selection");
                goto done;
            }
            path = Chip_XmlAttribute(root, "full-path");
        }
    if (!path || !RelativePath(path))
    {
        SDL_SetError("MusicXML: missing/invalid MXL score path");
        goto done;
    }
    for (size_t i = 0; i < count; ++i)
        if (SDL_strcmp(path, entries[i].name) == 0)
        {
            result = Extract(data, directory, &entries[i]);
            if (result)
                *output_size = entries[i].size;
            goto done;
        }
    SDL_SetError("MusicXML: score referenced by MXL container is missing");
done:
    Chip_XmlDestroy(container);
    for (size_t i = 0; i < count; ++i)
        SDL_free(entries[i].name);
    SDL_free(entries);
    return result;
}
