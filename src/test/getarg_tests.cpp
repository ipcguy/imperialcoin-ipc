#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-IPC");
    BOOST_CHECK(GetBoolArg("-IPC"));
    BOOST_CHECK(GetBoolArg("-IPC", false));
    BOOST_CHECK(GetBoolArg("-IPC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-IPCo"));
    BOOST_CHECK(!GetBoolArg("-IPCo", false));
    BOOST_CHECK(GetBoolArg("-IPCo", true));

    ResetArgs("-IPC=0");
    BOOST_CHECK(!GetBoolArg("-IPC"));
    BOOST_CHECK(!GetBoolArg("-IPC", false));
    BOOST_CHECK(!GetBoolArg("-IPC", true));

    ResetArgs("-IPC=1");
    BOOST_CHECK(GetBoolArg("-IPC"));
    BOOST_CHECK(GetBoolArg("-IPC", false));
    BOOST_CHECK(GetBoolArg("-IPC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noIPC");
    BOOST_CHECK(!GetBoolArg("-IPC"));
    BOOST_CHECK(!GetBoolArg("-IPC", false));
    BOOST_CHECK(!GetBoolArg("-IPC", true));

    ResetArgs("-noIPC=1");
    BOOST_CHECK(!GetBoolArg("-IPC"));
    BOOST_CHECK(!GetBoolArg("-IPC", false));
    BOOST_CHECK(!GetBoolArg("-IPC", true));

    ResetArgs("-IPC -noIPC");  // -IPC should win
    BOOST_CHECK(GetBoolArg("-IPC"));
    BOOST_CHECK(GetBoolArg("-IPC", false));
    BOOST_CHECK(GetBoolArg("-IPC", true));

    ResetArgs("-IPC=1 -noIPC=1");  // -IPC should win
    BOOST_CHECK(GetBoolArg("-IPC"));
    BOOST_CHECK(GetBoolArg("-IPC", false));
    BOOST_CHECK(GetBoolArg("-IPC", true));

    ResetArgs("-IPC=0 -noIPC=0");  // -IPC should win
    BOOST_CHECK(!GetBoolArg("-IPC"));
    BOOST_CHECK(!GetBoolArg("-IPC", false));
    BOOST_CHECK(!GetBoolArg("-IPC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--IPC=1");
    BOOST_CHECK(GetBoolArg("-IPC"));
    BOOST_CHECK(GetBoolArg("-IPC", false));
    BOOST_CHECK(GetBoolArg("-IPC", true));

    ResetArgs("--noIPC=1");
    BOOST_CHECK(!GetBoolArg("-IPC"));
    BOOST_CHECK(!GetBoolArg("-IPC", false));
    BOOST_CHECK(!GetBoolArg("-IPC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-IPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-IPC", "eleven"), "eleven");

    ResetArgs("-IPC -bar");
    BOOST_CHECK_EQUAL(GetArg("-IPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-IPC", "eleven"), "");

    ResetArgs("-IPC=");
    BOOST_CHECK_EQUAL(GetArg("-IPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-IPC", "eleven"), "");

    ResetArgs("-IPC=11");
    BOOST_CHECK_EQUAL(GetArg("-IPC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-IPC", "eleven"), "11");

    ResetArgs("-IPC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-IPC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-IPC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-IPC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-IPC", 0), 0);

    ResetArgs("-IPC -bar");
    BOOST_CHECK_EQUAL(GetArg("-IPC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-IPC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-IPC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-IPC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-IPC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--IPC");
    BOOST_CHECK_EQUAL(GetBoolArg("-IPC"), true);

    ResetArgs("--IPC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-IPC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noIPC");
    BOOST_CHECK(!GetBoolArg("-IPC"));
    BOOST_CHECK(!GetBoolArg("-IPC", true));
    BOOST_CHECK(!GetBoolArg("-IPC", false));

    ResetArgs("-noIPC=1");
    BOOST_CHECK(!GetBoolArg("-IPC"));
    BOOST_CHECK(!GetBoolArg("-IPC", true));
    BOOST_CHECK(!GetBoolArg("-IPC", false));

    ResetArgs("-noIPC=0");
    BOOST_CHECK(GetBoolArg("-IPC"));
    BOOST_CHECK(GetBoolArg("-IPC", true));
    BOOST_CHECK(GetBoolArg("-IPC", false));

    ResetArgs("-IPC --noIPC");
    BOOST_CHECK(GetBoolArg("-IPC"));

    ResetArgs("-noIPC -IPC"); // IPC always wins:
    BOOST_CHECK(GetBoolArg("-IPC"));
}

BOOST_AUTO_TEST_SUITE_END()
