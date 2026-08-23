// Grapple::Regex — the C surface the Lua/Ruby/C++ layers are built on.
#include <grapple/regex.h>

#include <gtest/gtest.h>

#include <string>

namespace
{

// A compiled pattern owns its match, so the tests treat one as a cursor.
struct Regex
{
    explicit Regex(const char *pattern, const char *flags = nullptr)
        : handle(Grapple_CompileRegex(pattern, flags))
    {
    }
    ~Regex() { Grapple_DestroyRegex(handle); }
    Regex(const Regex &) = delete;
    Regex &operator=(const Regex &) = delete;
    Grapple_Regex *handle;
};

TEST(Regex, CompilesAndReportsBadPatterns)
{
    Regex good("\\d+");
    ASSERT_NE(good.handle, nullptr);
    EXPECT_STREQ(Grapple_RegexPattern(good.handle), "\\d+");
    EXPECT_STREQ(Grapple_RegexFlags(good.handle), "");

    // The engine's own diagnostic reaches SDL_GetError, not a bare code.
    EXPECT_EQ(Grapple_CompileRegex("(unclosed", nullptr), nullptr);
    EXPECT_NE(std::string(SDL_GetError()).find("regex:"), std::string::npos);

    EXPECT_EQ(Grapple_CompileRegex("x", "q"), nullptr);
    EXPECT_NE(std::string(SDL_GetError()).find("unknown flag"), std::string::npos);
    EXPECT_EQ(Grapple_CompileRegex(nullptr, nullptr), nullptr);
}

TEST(Regex, SearchesAndCapturesGroups)
{
    Regex re("(\\w+)@(\\w+)");
    ASSERT_NE(re.handle, nullptr);

    EXPECT_EQ(Grapple_RegexGroupCount(re.handle), 0) << "no match yet";
    ASSERT_TRUE(Grapple_RegexSearch(re.handle, "mail bob@example now", 0));
    EXPECT_EQ(Grapple_RegexGroupCount(re.handle), 3);
    EXPECT_STREQ(Grapple_RegexGroup(re.handle, 0), "bob@example");
    EXPECT_STREQ(Grapple_RegexGroup(re.handle, 1), "bob");
    EXPECT_STREQ(Grapple_RegexGroup(re.handle, 2), "example");
    EXPECT_EQ(Grapple_RegexGroupBegin(re.handle, 1), 5);
    EXPECT_EQ(Grapple_RegexGroupEnd(re.handle, 1), 8);
    EXPECT_EQ(Grapple_RegexGroup(re.handle, 7), nullptr);

    // A failed search is false without being an error, and clears the match.
    EXPECT_FALSE(Grapple_RegexSearch(re.handle, "nothing here", 0));
    EXPECT_EQ(Grapple_RegexGroupCount(re.handle), 0);
}

TEST(Regex, HonoursFlagsAndUtf8)
{
    Regex insensitive("hello", "i");
    ASSERT_NE(insensitive.handle, nullptr);
    EXPECT_TRUE(Grapple_RegexSearch(insensitive.handle, "Say HELLO", 0));

    Regex dotall("a.b", "m"); // Ruby's /m: dot spans newlines
    ASSERT_NE(dotall.handle, nullptr);
    EXPECT_TRUE(Grapple_RegexSearch(dotall.handle, "a\nb", 0));
    Regex plain("a.b");
    ASSERT_NE(plain.handle, nullptr);
    EXPECT_FALSE(Grapple_RegexSearch(plain.handle, "a\nb", 0));

    Regex extended("\\d+  # the number\n", "x");
    ASSERT_NE(extended.handle, nullptr);
    EXPECT_TRUE(Grapple_RegexSearch(extended.handle, "abc 42", 0));
    EXPECT_STREQ(Grapple_RegexGroup(extended.handle, 0), "42");

    // UTF-8 is the subject encoding: \w matches non-ASCII letters, and the
    // offsets that come back are byte offsets into the original text.
    Regex word("\\w+");
    ASSERT_NE(word.handle, nullptr);
    ASSERT_TRUE(Grapple_RegexSearch(word.handle, "  \xc3\xa9t\xc3\xa9  ", 0));
    EXPECT_STREQ(Grapple_RegexGroup(word.handle, 0), "\xc3\xa9t\xc3\xa9");
    EXPECT_EQ(Grapple_RegexGroupBegin(word.handle, 0), 2);
    EXPECT_EQ(Grapple_RegexGroupEnd(word.handle, 0), 7) << "5 bytes, 3 characters";
}

TEST(Regex, ResolvesNamedGroups)
{
    Regex re("(?<year>\\d{4})-(?<month>\\d{2})");
    ASSERT_NE(re.handle, nullptr);
    EXPECT_EQ(Grapple_RegexNamedGroupCount(re.handle), 2);
    EXPECT_STREQ(Grapple_RegexNamedGroupName(re.handle, 0), "year");
    EXPECT_STREQ(Grapple_RegexNamedGroupName(re.handle, 1), "month");
    EXPECT_EQ(Grapple_RegexNamedGroupName(re.handle, 2), nullptr);

    ASSERT_TRUE(Grapple_RegexSearch(re.handle, "on 2026-08-14", 0));
    const int year = Grapple_RegexNamedGroup(re.handle, "year");
    ASSERT_GT(year, 0);
    EXPECT_STREQ(Grapple_RegexGroup(re.handle, year), "2026");
    EXPECT_EQ(Grapple_RegexNamedGroup(re.handle, "day"), -1);
}

// Anchored matching is what a scanning loop needs: does it match *here*.
TEST(Regex, MatchAtDoesNotSearchForward)
{
    Regex re("\\d+");
    ASSERT_NE(re.handle, nullptr);
    EXPECT_FALSE(Grapple_RegexMatchAt(re.handle, "ab12", 0));
    EXPECT_TRUE(Grapple_RegexMatchAt(re.handle, "ab12", 2));
    EXPECT_TRUE(Grapple_RegexSearch(re.handle, "ab12", 0));

    EXPECT_FALSE(Grapple_RegexSearch(re.handle, "ab12", 99));
    EXPECT_NE(std::string(SDL_GetError()).find("outside"), std::string::npos);
}

TEST(Regex, ReplacesWithGroupReferences)
{
    Regex re("(\\w+)@(\\w+)");
    ASSERT_NE(re.handle, nullptr);
    EXPECT_STREQ(Grapple_RegexReplace(re.handle, "bob@example and amy@test", "\\2/\\1", true),
                 "example/bob and test/amy");
    EXPECT_STREQ(Grapple_RegexReplace(re.handle, "bob@example and amy@test", "\\2/\\1", false),
                 "example/bob and amy@test")
        << "sub replaces only the first";

    Regex digits("\\d");
    ASSERT_NE(digits.handle, nullptr);
    EXPECT_STREQ(Grapple_RegexReplace(digits.handle, "a1b2", "#", true), "a#b#");
    EXPECT_STREQ(Grapple_RegexReplace(digits.handle, "no digits", "#", true), "no digits")
        << "no match yields the subject unchanged";
    EXPECT_STREQ(Grapple_RegexReplace(digits.handle, "7", "\\\\", true), "\\")
        << "a doubled backslash is a literal one";

    // An empty match must still make progress rather than spin.
    Regex empty("x*");
    ASSERT_NE(empty.handle, nullptr);
    EXPECT_STREQ(Grapple_RegexReplace(empty.handle, "ab", "-", true), "-a-b-");
}

TEST(Regex, EscapesMetacharacters)
{
    char *quoted = Grapple_RegexEscape("1+1 (really?)");
    ASSERT_NE(quoted, nullptr);
    Regex re(quoted);
    ASSERT_NE(re.handle, nullptr);
    EXPECT_TRUE(Grapple_RegexSearch(re.handle, "check 1+1 (really?) now", 0));
    EXPECT_FALSE(Grapple_RegexSearch(re.handle, "check 111 reallyx now", 0));
    SDL_free(quoted);
    EXPECT_EQ(Grapple_RegexEscape(nullptr), nullptr);
}

TEST(Regex, HandlesNullArgumentsAndDestroy)
{
    Grapple_DestroyRegex(nullptr);
    EXPECT_FALSE(Grapple_RegexSearch(nullptr, "x", 0));
    EXPECT_EQ(Grapple_RegexGroupCount(nullptr), 0);
    EXPECT_EQ(Grapple_RegexGroup(nullptr, 0), nullptr);
    EXPECT_EQ(Grapple_RegexGroupBegin(nullptr, 0), -1);
    EXPECT_EQ(Grapple_RegexGroupEnd(nullptr, 0), -1);
    EXPECT_EQ(Grapple_RegexNamedGroup(nullptr, "x"), -1);
    EXPECT_EQ(Grapple_RegexNamedGroupCount(nullptr), 0);
    EXPECT_EQ(Grapple_RegexNamedGroupName(nullptr, 0), nullptr);
    EXPECT_EQ(Grapple_RegexPattern(nullptr), nullptr);
    EXPECT_EQ(Grapple_RegexFlags(nullptr), nullptr);
    EXPECT_EQ(Grapple_RegexReplace(nullptr, "a", "b", true), nullptr);

    Regex re("x");
    ASSERT_NE(re.handle, nullptr);
    EXPECT_FALSE(Grapple_RegexSearch(re.handle, nullptr, 0));
}

} // namespace
