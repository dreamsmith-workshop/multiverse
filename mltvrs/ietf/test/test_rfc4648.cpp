#include <mltvrs/ietf/rfc4648.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

CATCH_SCENARIO("RFC-4648 base-64 conversion works as expected")
{
    CATCH_GIVEN("an un-encoded string")
    {
        using namespace std::string_view_literals;

        const auto test_data = GENERATE(
            std::pair{"mXYcHgibU"sv, "bVhZY0hnaWJV"sv},
            std::pair{"slkHpm9bI"sv, "c2xrSHBtOWJJ"sv},
            std::pair{"8YNf8yBkj"sv, "OFlOZjh5Qmtq"sv},
            std::pair{"GDGTb7NHj"sv, "R0RHVGI3Tkhq"sv},
            std::pair{"x53J0E6Ub"sv, "eDUzSjBFNlVi"sv},
            std::pair{"AqDiHA8vZ"sv, "QXFEaUhBOHZa"sv},
            std::pair{"uGJX8rmww"sv, "dUdKWDhybXd3"sv},
            std::pair{"qkjibBZEF"sv, "cWtqaWJCWkVG"sv},
            std::pair{"fRokx7he3"sv, "ZlJva3g3aGUz"sv},
            std::pair{"XtAdmpyvP"sv, "WHRBZG1weXZQ"sv},
            std::pair{"kmLJddqAcD"sv, "a21MSmRkcUFjRA=="sv},
            std::pair{"7Ypa9lj8c5"sv, "N1lwYTlsajhjNQ=="sv},
            std::pair{"qTUiumAy3C"sv, "cVRVaXVtQXkzQw=="sv},
            std::pair{"ktrgNiEM7D"sv, "a3RyZ05pRU03RA=="sv},
            std::pair{"S4j4cMbeWm"sv, "UzRqNGNNYmVXbQ=="sv},
            std::pair{"z426xp1Kad"sv, "ejQyNnhwMUthZA=="sv},
            std::pair{"5Ydv7NOs4u"sv, "NVlkdjdOT3M0dQ=="sv},
            std::pair{"JW7ZDFxraz"sv, "Slc3WkRGeHJheg=="sv},
            std::pair{"up3T7CVYWW"sv, "dXAzVDdDVllXVw=="sv},
            std::pair{"hbkwRO9Fli"sv, "aGJrd1JPOUZsaQ=="sv},
            std::pair{"JFPON6wI2Qt"sv, "SkZQT042d0kyUXQ="sv},
            std::pair{"v5Dwim8GM8y"sv, "djVEd2ltOEdNOHk="sv},
            std::pair{"RGWRRDcjTDO"sv, "UkdXUlJEY2pURE8="sv},
            std::pair{"3xDET6OD1I2"sv, "M3hERVQ2T0QxSTI="sv},
            std::pair{"AVdhifwtcHu"sv, "QVZkaGlmd3RjSHU="sv},
            std::pair{"5LpxVhxWqeg"sv, "NUxweFZoeFdxZWc="sv},
            std::pair{"XwmheuzbWOv"sv, "WHdtaGV1emJXT3Y="sv},
            std::pair{"GexdKzkUrE8"sv, "R2V4ZEt6a1VyRTg="sv},
            std::pair{"YlQZF8i5A2Q"sv, "WWxRWkY4aTVBMlE="sv},
            std::pair{"94HpDFzbSgs"sv, "OTRIcERGemJTZ3M="sv});

        CATCH_THEN("the base-64 encoded string matches expectations")
        {
            CATCH_REQUIRE(test_data.second == mltvrs::ietf::encode_base64(test_data.first));
            CATCH_REQUIRE(
                test_data.first == mltvrs::ietf::decode_base64<std::string>(test_data.second));
        }
    }
}
