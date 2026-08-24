/**
 * @file version_test.cpp
 * @brief Unit tests for grapple::Version.
 */

#include "grapple/version.hpp"

#include <gtest/gtest.h>
#include <string>
#include <string_view>

namespace
{

TEST(VersionTest, IsNonEmpty)
{
    const std::string_view version = grapple::Version();
    EXPECT_FALSE(version.empty());
}

TEST(VersionTest, MatchesComponentConstants)
{
    // Version() is generated from the root VERSION file; constants must agree.
    const std::string expected = std::to_string(grapple::kVersionMajor) + "." +
                                 std::to_string(grapple::kVersionMinor) + "." +
                                 std::to_string(grapple::kVersionPatch);
    EXPECT_EQ(std::string_view{grapple::Version()}, expected);
    EXPECT_GE(grapple::kVersionMajor, 0);
    EXPECT_GE(grapple::kVersionMinor, 0);
    EXPECT_GE(grapple::kVersionPatch, 0);
}

TEST(VersionTest, HasThreeNumericComponents)
{
    const std::string_view version = grapple::Version();
    EXPECT_NE(version.find('.'), std::string_view::npos);
    EXPECT_NE(version.rfind('.'), std::string_view::npos);
    EXPECT_NE(version.find('.'), version.rfind('.'));
}

} // namespace
