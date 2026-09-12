#include "chip_xml.h"

#include <gtest/gtest.h>
#include <memory>
#include <string>

namespace
{
using Xml = std::unique_ptr<ChipXmlNode, decltype(&Chip_XmlDestroy)>;
Xml Parse(const std::string &text)
{
    return Xml(Chip_XmlParse(text.data(), text.size()), Chip_XmlDestroy);
}
} // namespace

TEST(ChipXml, ParsesNamespaceTextAttributesAndCommonDoctype)
{
    const auto xml = Parse("<?xml version='1.0'?><!DOCTYPE score-partwise SYSTEM "
                           "'https://invalid.example/score.dtd'><score-partwise>"
                           "<part id='P1'><name>Lead &amp; <![CDATA[Chords]]></name></part>"
                           "</score-partwise>");
    ASSERT_TRUE(xml) << SDL_GetError();
    const auto *part = Chip_XmlChild(xml.get(), "part");
    ASSERT_NE(part, nullptr);
    EXPECT_STREQ(Chip_XmlAttribute(part, "id"), "P1");
    EXPECT_STREQ(Chip_XmlText(part, "name"), "Lead & Chords");
    const auto namespaced =
        Parse("<x:score-partwise xmlns:x='urn:test'><x:part/></x:score-partwise>");
    ASSERT_TRUE(namespaced);
    EXPECT_NE(Chip_XmlChild(namespaced.get(), "part"), nullptr);
}

TEST(ChipXml, RejectsEntitiesTruncationAndDeepDocuments)
{
    EXPECT_FALSE(Parse("<!DOCTYPE a [<!ENTITY b 'expanded'>]><a>&b;</a>"));
    EXPECT_FALSE(Parse("<!DOCTYPE a [<!ENTITY b SYSTEM 'file:///invalid'>]><a>&b;</a>"));
    EXPECT_FALSE(Parse("<a><b></a>"));
    EXPECT_FALSE(Parse("<a>"));
    std::string deep;
    for (int i = 0; i < 130; ++i)
        deep += "<a>";
    for (int i = 0; i < 130; ++i)
        deep += "</a>";
    EXPECT_FALSE(Parse(deep));
}

TEST(ChipXml, ReadsGuitarProFixtureWithoutNetworkOrEntityExpansion)
{
    size_t size = 0;
    void *data = SDL_LoadFile(MIXER_TEST_ASSETS_DIR "/c64-composition-named.xml", &size);
    ASSERT_NE(data, nullptr);
    const Xml xml(Chip_XmlParse(data, size), Chip_XmlDestroy);
    SDL_free(data);
    ASSERT_TRUE(xml) << SDL_GetError();
    EXPECT_STREQ(xml->name, "score-partwise");
    EXPECT_STREQ(Chip_XmlAttribute(xml.get(), "version"), "2.0");
    int parts = 0;
    for (const auto *node = xml->children; node; node = node->next)
        if (SDL_strcmp(node->name, "part") == 0)
            ++parts;
    EXPECT_EQ(parts, 4);
}
