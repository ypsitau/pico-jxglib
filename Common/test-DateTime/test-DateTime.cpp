#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

// Struct for test cases

// Test function for DateTime::Compare using a test case table
void test_Compare()
{
    struct CompareTestCase {
        DateTime dt1;
        DateTime dt2;
        int expected;
        const char* description;
    };
    CompareTestCase testCases[] = {
        // Year
        {{2024,6,3,12,0,0,0}, {2025,6,3,12,0,0,0}, -1, "Year: dt1 < dt2"},
        {{2025,6,3,12,0,0,0}, {2024,6,3,12,0,0,0},  1, "Year: dt1 > dt2"},
        // Month
        {{2025,5,3,12,0,0,0}, {2025,6,3,12,0,0,0}, -1, "Month: dt1 < dt2"},
        {{2025,6,3,12,0,0,0}, {2025,5,3,12,0,0,0},  1, "Month: dt1 > dt2"},
        // Day
        {{2025,6,2,12,0,0,0}, {2025,6,3,12,0,0,0}, -1, "Day: dt1 < dt2"},
        {{2025,6,3,12,0,0,0}, {2025,6,2,12,0,0,0},  1, "Day: dt1 > dt2"},
        // Hour
        {{2025,6,3,11,0,0,0}, {2025,6,3,12,0,0,0}, -1, "Hour: dt1 < dt2"},
        {{2025,6,3,12,0,0,0}, {2025,6,3,11,0,0,0},  1, "Hour: dt1 > dt2"},
        // Minute
        {{2025,6,3,12,10,0,0}, {2025,6,3,12,11,0,0}, -1, "Minute: dt1 < dt2"},
        {{2025,6,3,12,11,0,0}, {2025,6,3,12,10,0,0},  1, "Minute: dt1 > dt2"},
        // Second
        {{2025,6,3,12,0,10,0}, {2025,6,3,12,0,11,0}, -1, "Second: dt1 < dt2"},
        {{2025,6,3,12,0,11,0}, {2025,6,3,12,0,10,0},  1, "Second: dt1 > dt2"},
        // Millisecond
        {{2025,6,3,12,0,0,100}, {2025,6,3,12,0,0,200}, -1, "Msec: dt1 < dt2"},
        {{2025,6,3,12,0,0,200}, {2025,6,3,12,0,0,100},  1, "Msec: dt1 > dt2"},
        // Identical
        {{2025,6,3,12,0,0,123}, {2025,6,3,12,0,0,123}, 0, "Identical: dt1 == dt2"},
    };

    int numCases = sizeof(testCases) / sizeof(testCases[0]);
    for (int i = 0; i < numCases; ++i) {
        int result = DateTime::Compare(testCases[i].dt1, testCases[i].dt2);
        printf("%-25s | result: %2d | expected: %2d | %s\n",
            testCases[i].description, result, testCases[i].expected,
            (result == testCases[i].expected) ? "OK" : "NG");
    }
}

int main()
{
    ::stdio_init_all();

    test_Compare();

    for (;;) ::tight_loop_contents();
}
