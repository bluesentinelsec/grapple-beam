/* Original Grapple code (zlib). Bounded XML document used by score importers. */
#ifndef GRAPPLE_CHIP_XML_H
#define GRAPPLE_CHIP_XML_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ChipXmlNode
    {
        char *name;
        char *text;
        size_t text_size;
        char **attributes;
        struct ChipXmlNode *parent;
        struct ChipXmlNode *children;
        struct ChipXmlNode *last;
        struct ChipXmlNode *next;
        unsigned long line;
    } ChipXmlNode;

    ChipXmlNode *Chip_XmlParse(const void *data, size_t size);
    void Chip_XmlDestroy(ChipXmlNode *node);
    const ChipXmlNode *Chip_XmlChild(const ChipXmlNode *node, const char *name);
    const char *Chip_XmlAttribute(const ChipXmlNode *node, const char *name);
    const char *Chip_XmlText(const ChipXmlNode *node, const char *name);

#ifdef __cplusplus
}
#endif
#endif
