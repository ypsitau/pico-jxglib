#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

// Test function for DateTime::Compare (checks all date, time, and msec fields)
void test_Compare() {
    // Year difference
    DateTime dt_y1 = {2024, 6, 3, 12, 0, 0, 0};
    DateTime dt_y2 = {2025, 6, 3, 12, 0, 0, 0};
    printf("Year: Compare(dt_y1, dt_y2) = %d (expected: -1)\n", DateTime::Compare(dt_y1, dt_y2));
    printf("Year: Compare(dt_y2, dt_y1) = %d (expected: 1)\n", DateTime::Compare(dt_y2, dt_y1));

    // Month difference
    DateTime dt_m1 = {2025, 5, 3, 12, 0, 0, 0};
    DateTime dt_m2 = {2025, 6, 3, 12, 0, 0, 0};
    printf("Month: Compare(dt_m1, dt_m2) = %d (expected: -1)\n", DateTime::Compare(dt_m1, dt_m2));
    printf("Month: Compare(dt_m2, dt_m1) = %d (expected: 1)\n", DateTime::Compare(dt_m2, dt_m1));

    // Day difference
    DateTime dt_d1 = {2025, 6, 2, 12, 0, 0, 0};
    DateTime dt_d2 = {2025, 6, 3, 12, 0, 0, 0};
    printf("Day: Compare(dt_d1, dt_d2) = %d (expected: -1)\n", DateTime::Compare(dt_d1, dt_d2));
    printf("Day: Compare(dt_d2, dt_d1) = %d (expected: 1)\n", DateTime::Compare(dt_d2, dt_d1));

    // Hour difference
    DateTime dt_h1 = {2025, 6, 3, 11, 0, 0, 0};
    DateTime dt_h2 = {2025, 6, 3, 12, 0, 0, 0};
    printf("Hour: Compare(dt_h1, dt_h2) = %d (expected: -1)\n", DateTime::Compare(dt_h1, dt_h2));
    printf("Hour: Compare(dt_h2, dt_h1) = %d (expected: 1)\n", DateTime::Compare(dt_h2, dt_h1));

    // Minute difference
    DateTime dt_min1 = {2025, 6, 3, 12, 10, 0, 0};
    DateTime dt_min2 = {2025, 6, 3, 12, 11, 0, 0};
    printf("Minute: Compare(dt_min1, dt_min2) = %d (expected: -1)\n", DateTime::Compare(dt_min1, dt_min2));
    printf("Minute: Compare(dt_min2, dt_min1) = %d (expected: 1)\n", DateTime::Compare(dt_min2, dt_min1));

    // Second difference
    DateTime dt_s1 = {2025, 6, 3, 12, 0, 10, 0};
    DateTime dt_s2 = {2025, 6, 3, 12, 0, 11, 0};
    printf("Second: Compare(dt_s1, dt_s2) = %d (expected: -1)\n", DateTime::Compare(dt_s1, dt_s2));
    printf("Second: Compare(dt_s2, dt_s1) = %d (expected: 1)\n", DateTime::Compare(dt_s2, dt_s1));

    // Millisecond difference
    DateTime dt_ms1 = {2025, 6, 3, 12, 0, 0, 100};
    DateTime dt_ms2 = {2025, 6, 3, 12, 0, 0, 200};
    printf("Msec: Compare(dt_ms1, dt_ms2) = %d (expected: -1)\n", DateTime::Compare(dt_ms1, dt_ms2));
    printf("Msec: Compare(dt_ms2, dt_ms1) = %d (expected: 1)\n", DateTime::Compare(dt_ms2, dt_ms1));

    // Identical
    DateTime dt_eq1 = {2025, 6, 3, 12, 0, 0, 123};
    DateTime dt_eq2 = {2025, 6, 3, 12, 0, 0, 123};
    printf("Identical: Compare(dt_eq1, dt_eq2) = %d (expected: 0)\n", DateTime::Compare(dt_eq1, dt_eq2));
}

int main()
{
    ::stdio_init_all();

    test_Compare();

	for (;;) ::tight_loop_contents();
}
