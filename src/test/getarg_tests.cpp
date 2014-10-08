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
    ResetArgs("-ECHO");
    BOOST_CHECK(GetBoolArg("-ECHO"));
    BOOST_CHECK(GetBoolArg("-ECHO", false));
    BOOST_CHECK(GetBoolArg("-ECHO", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ECHOo"));
    BOOST_CHECK(!GetBoolArg("-ECHOo", false));
    BOOST_CHECK(GetBoolArg("-ECHOo", true));

    ResetArgs("-ECHO=0");
    BOOST_CHECK(!GetBoolArg("-ECHO"));
    BOOST_CHECK(!GetBoolArg("-ECHO", false));
    BOOST_CHECK(!GetBoolArg("-ECHO", true));

    ResetArgs("-ECHO=1");
    BOOST_CHECK(GetBoolArg("-ECHO"));
    BOOST_CHECK(GetBoolArg("-ECHO", false));
    BOOST_CHECK(GetBoolArg("-ECHO", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noECHO");
    BOOST_CHECK(!GetBoolArg("-ECHO"));
    BOOST_CHECK(!GetBoolArg("-ECHO", false));
    BOOST_CHECK(!GetBoolArg("-ECHO", true));

    ResetArgs("-noECHO=1");
    BOOST_CHECK(!GetBoolArg("-ECHO"));
    BOOST_CHECK(!GetBoolArg("-ECHO", false));
    BOOST_CHECK(!GetBoolArg("-ECHO", true));

    ResetArgs("-ECHO -noECHO");  // -ECHO should win
    BOOST_CHECK(GetBoolArg("-ECHO"));
    BOOST_CHECK(GetBoolArg("-ECHO", false));
    BOOST_CHECK(GetBoolArg("-ECHO", true));

    ResetArgs("-ECHO=1 -noECHO=1");  // -ECHO should win
    BOOST_CHECK(GetBoolArg("-ECHO"));
    BOOST_CHECK(GetBoolArg("-ECHO", false));
    BOOST_CHECK(GetBoolArg("-ECHO", true));

    ResetArgs("-ECHO=0 -noECHO=0");  // -ECHO should win
    BOOST_CHECK(!GetBoolArg("-ECHO"));
    BOOST_CHECK(!GetBoolArg("-ECHO", false));
    BOOST_CHECK(!GetBoolArg("-ECHO", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ECHO=1");
    BOOST_CHECK(GetBoolArg("-ECHO"));
    BOOST_CHECK(GetBoolArg("-ECHO", false));
    BOOST_CHECK(GetBoolArg("-ECHO", true));

    ResetArgs("--noECHO=1");
    BOOST_CHECK(!GetBoolArg("-ECHO"));
    BOOST_CHECK(!GetBoolArg("-ECHO", false));
    BOOST_CHECK(!GetBoolArg("-ECHO", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", "eleven"), "eleven");

    ResetArgs("-ECHO -bar");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", "eleven"), "");

    ResetArgs("-ECHO=");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", "eleven"), "");

    ResetArgs("-ECHO=11");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", "eleven"), "11");

    ResetArgs("-ECHO=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ECHO", 0), 0);

    ResetArgs("-ECHO -bar");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ECHO=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ECHO=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ECHO");
    BOOST_CHECK_EQUAL(GetBoolArg("-ECHO"), true);

    ResetArgs("--ECHO=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ECHO", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noECHO");
    BOOST_CHECK(!GetBoolArg("-ECHO"));
    BOOST_CHECK(!GetBoolArg("-ECHO", true));
    BOOST_CHECK(!GetBoolArg("-ECHO", false));

    ResetArgs("-noECHO=1");
    BOOST_CHECK(!GetBoolArg("-ECHO"));
    BOOST_CHECK(!GetBoolArg("-ECHO", true));
    BOOST_CHECK(!GetBoolArg("-ECHO", false));

    ResetArgs("-noECHO=0");
    BOOST_CHECK(GetBoolArg("-ECHO"));
    BOOST_CHECK(GetBoolArg("-ECHO", true));
    BOOST_CHECK(GetBoolArg("-ECHO", false));

    ResetArgs("-ECHO --noECHO");
    BOOST_CHECK(GetBoolArg("-ECHO"));

    ResetArgs("-noECHO -ECHO"); // ECHO always wins:
    BOOST_CHECK(GetBoolArg("-ECHO"));
}

BOOST_AUTO_TEST_SUITE_END()
