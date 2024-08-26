#include <catch2/catch_test_macros.hpp>
#include "calc/Calc.h"

TEST_CASE( "Addition", "[calc]" ) {
    Calc calc;
    REQUIRE(calc.add(2, 3) == 5);
}
