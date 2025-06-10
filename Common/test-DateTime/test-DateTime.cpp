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

// Test function for DateTime::CalcDayOfWeek using a test case table
void test_CalcDayOfWeek()
{
    struct DayOfWeekTestCase {
        DateTime dt;
        int expected; // 0: Sunday, 1: Monday, ..., 6: Saturday
        const char* description;
    };
    DayOfWeekTestCase testCases[] = {
        // 2024
        {{2024,  1,  1, 0,0,0,0}, 1, "2024-01-01 (Monday)"},
        {{2024,  2, 29, 0,0,0,0}, 4, "2024-02-29 (Thursday)"},
        {{2024,  3,  1, 0,0,0,0}, 5, "2024-03-01 (Friday)"},
        {{2024,  4,  1, 0,0,0,0}, 1, "2024-04-01 (Monday)"},
        {{2024,  5,  1, 0,0,0,0}, 3, "2024-05-01 (Wednesday)"},
        {{2024,  6,  1, 0,0,0,0}, 6, "2024-06-01 (Saturday)"},
        {{2024,  7,  1, 0,0,0,0}, 1, "2024-07-01 (Monday)"},
        {{2024,  8,  1, 0,0,0,0}, 4, "2024-08-01 (Thursday)"},
        {{2024,  9,  1, 0,0,0,0}, 0, "2024-09-01 (Sunday)"},
        {{2024, 10,  1, 0,0,0,0}, 2, "2024-10-01 (Tuesday)"},
        {{2024, 11,  1, 0,0,0,0}, 5, "2024-11-01 (Friday)"},
        {{2024, 12,  1, 0,0,0,0}, 0, "2024-12-01 (Sunday)"},
        // 2025
        {{2025,  1,  1, 0,0,0,0}, 3, "2025-01-01 (Wednesday)"},
        {{2025,  2, 28, 0,0,0,0}, 5, "2025-02-28 (Friday)"},
        {{2025,  3,  1, 0,0,0,0}, 6, "2025-03-01 (Saturday)"},
        {{2025,  4,  1, 0,0,0,0}, 2, "2025-04-01 (Tuesday)"},
        {{2025,  5,  1, 0,0,0,0}, 4, "2025-05-01 (Thursday)"},
        {{2025,  6,  1, 0,0,0,0}, 0, "2025-06-01 (Sunday)"},
        {{2025,  7,  1, 0,0,0,0}, 2, "2025-07-01 (Tuesday)"},
        {{2025,  8,  1, 0,0,0,0}, 5, "2025-08-01 (Friday)"},
        {{2025,  9,  1, 0,0,0,0}, 1, "2025-09-01 (Monday)"},
        {{2025, 10,  1, 0,0,0,0}, 3, "2025-10-01 (Wednesday)"},
        {{2025, 11,  1, 0,0,0,0}, 6, "2025-11-01 (Saturday)"},
        {{2025, 12,  1, 0,0,0,0}, 1, "2025-12-01 (Monday)"},
        // 2030
        {{2030,  1,  1, 0,0,0,0}, 2, "2030-01-01 (Tuesday)"},
        {{2030,  2, 28, 0,0,0,0}, 4, "2030-02-28 (Thursday)"},
        {{2030,  3,  1, 0,0,0,0}, 5, "2030-03-01 (Friday)"},
        {{2030,  4,  1, 0,0,0,0}, 1, "2030-04-01 (Monday)"},
        {{2030,  5,  1, 0,0,0,0}, 3, "2030-05-01 (Wednesday)"},
        {{2030,  6,  1, 0,0,0,0}, 6, "2030-06-01 (Saturday)"},
        {{2030,  7,  1, 0,0,0,0}, 1, "2030-07-01 (Monday)"},
        {{2030,  8,  1, 0,0,0,0}, 4, "2030-08-01 (Thursday)"},
        {{2030,  9,  1, 0,0,0,0}, 0, "2030-09-01 (Sunday)"},
        {{2030, 10,  1, 0,0,0,0}, 2, "2030-10-01 (Tuesday)"},
        {{2030, 11,  1, 0,0,0,0}, 5, "2030-11-01 (Friday)"},
        {{2030, 12,  1, 0,0,0,0}, 0, "2030-12-01 (Sunday)"},
        // 2040
        {{2040,  1,  1, 0,0,0,0}, 0, "2040-01-01 (Sunday)"},
        {{2040,  2, 29, 0,0,0,0}, 3, "2040-02-29 (Thursday)"},
        {{2040,  3,  1, 0,0,0,0}, 4, "2040-03-01 (Friday)"},
        {{2040,  4,  1, 0,0,0,0}, 0, "2040-04-01 (Sunday)"},
        {{2040,  5,  1, 0,0,0,0}, 2, "2040-05-01 (Wednesday)"},
        {{2040,  6,  1, 0,0,0,0}, 5, "2040-06-01 (Saturday)"},
        {{2040,  7,  1, 0,0,0,0}, 0, "2040-07-01 (Sunday)"},
        {{2040,  8,  1, 0,0,0,0}, 3, "2040-08-01 (Thursday)"},
        {{2040,  9,  1, 0,0,0,0}, 6, "2040-09-01 (Saturday)"},
        {{2040, 10,  1, 0,0,0,0}, 1, "2040-10-01 (Tuesday)"},
        {{2040, 11,  1, 0,0,0,0}, 4, "2040-11-01 (Friday)"},
        {{2040, 12,  1, 0,0,0,0}, 6, "2040-12-01 (Saturday)"},
        // others
        {{2000,  1,  1, 0,0,0,0}, 6, "2000-01-01 (Saturday)"},
        {{2010,  1,  1, 0,0,0,0}, 5, "2010-01-01 (Friday)"},
        {{2020,  2, 29, 0,0,0,0}, 6, "2020-02-29 (Saturday)"},
        {{2035, 12, 25, 0,0,0,0}, 2, "2035-12-25 (Tuesday)"},
        {{1970, 1, 1, 0, 0, 0, 0}, 4, "1970-01-01 (Thursday)"},  // Unix epoch
        {{1999,12,31, 0, 0, 0, 0}, 5, "1999-12-31 (Friday)"},    // Y2K eve
    };

    int numCases = sizeof(testCases) / sizeof(testCases[0]);
    for (int i = 0; i < numCases; ++i) {
        int result = testCases[i].dt.CalcDayOfWeek();
        printf("%-30s | result: %d | expected: %d | %s\n",
            testCases[i].description, result, testCases[i].expected,
            (result == testCases[i].expected) ? "OK" : "NG");
    }
}

// Test function for DateTime::Parse using a test case table
void test_Parse()
{
    struct ParseTestCase {
        const char* str;
        DateTime expected;
        bool expectSuccess;
        const char* description;
    };
    ParseTestCase testCases[] = {
        //{"2024-06-10 12:34:56.789", {2024,6,10,12,34,56,789}, true, "Date, time, msec"},
#if 1
        // Valid patterns
        {"2024-06-10", {2024,6,10,0,0,0,0}, true, "Date only (dash)"},
        {"2024/06/10", {2024,6,10,0,0,0,0}, true, "Date only (slash)"},
        {"2024-06-10 12:34:56", {2024,6,10,12,34,56,0}, true, "Date and time"},
        {"2024/06/10 01:02:03", {2024,6,10,1,2,3,0}, true, "Date and time (slash)"},
        {"2024-06-10 12:34:56.789", {2024,6,10,12,34,56,789}, true, "Date, time, msec"},
        {"2024/06/10 12:34:56.007", {2024,6,10,12,34,56,7}, true, "Date, time, msec (leading zero)"},
        {"2024-06-10T23:59:59.999", {2024,6,10,23,59,59,999}, true, "ISO8601 T separator"},
        {"2024-06-10 00:00:00.001", {2024,6,10,0,0,0,1}, true, "Midnight with msec"},
        {"2024-06-10 23:59:59.1", {2024,6,10,23,59,59,100}, true, "One digit msec"},
        {"2024-06-10 23:59:59.12", {2024,6,10,23,59,59,120}, true, "Two digit msec"},
        {"2024-06-10 23:59:59,123", {2024,6,10,23,59,59,123}, true, "Comma msec"},
        {"2024-06-10T00:00:00", {2024,6,10,0,0,0,0}, true, "ISO8601 T no msec"},
        {"2024-06-10 12:34:56,000", {2024,6,10,12,34,56,0}, true, "Comma msec zero"},
        {"2024-06-10 12:34:56.000", {2024,6,10,12,34,56,0}, true, "Dot msec zero"},
        {"2024-06-10 12:34:56.099", {2024,6,10,12,34,56,99}, true, "Dot msec two digits"},
        {"2024-06-10 12:34:56.9", {2024,6,10,12,34,56,900}, true, "Dot msec one digit"},
        {"2024-06-10 12:34:56.090", {2024,6,10,12,34,56,90}, true, "Dot msec two digits (leading zero)"},
        {"2024-06-10 12:34:56.009", {2024,6,10,12,34,56,9}, true, "Dot msec two digits (trailing zero)"},
        {"2024-06-10 12:34:56.000", {2024,6,10,12,34,56,0}, true, "Dot msec all zero"},
        {"2024-06-10 12:34:56", {2024,6,10,12,34,56,0}, true, "No msec"},
        {"2024-06-10T12:34:56", {2024,6,10,12,34,56,0}, true, "T separator, no msec"},
        {"2024-06-10 00:00:00", {2024,6,10,0,0,0,0}, true, "Midnight"},
        {"2024-06-10 23:59:59", {2024,6,10,23,59,59,0}, true, "Last second of day"},
        {"2024-06-10 12:34:56.123", {2024,6,10,12,34,56,123}, true, "Dot msec"},
        {"2024-6-1", {2024,6,1,0,0,0,0}, true, "One-digit month and day format"},
        {"2024-6-10", {2024,6,10,0,0,0,0}, true, "One-digit month format"},
        {"2024-06-1", {2024,6,1,0,0,0,0}, true, "One-digit day format"},
        {"2024-6-1 12:34:56", {2024,6,1,12,34,56,0}, true, "One-digit month and day format"},
        {"2024-6-10 12:34:56", {2024,6,10,12,34,56,0}, true, "One-digit month format"},
        {"2024-06-1 12:34:56", {2024,6,1,12,34,56,0}, true, "One-digit day format"},
        {"2024-06-10 12:34", {2024,6,10,12,34,0,0}, true, "Missing seconds"},
        // Invalid patterns
        {"abcd-ef-gh", {}, false, "Non-numeric"},
        {"2024-06-10 12:34:56.x", {}, false, "Invalid msec"},
        {"", {}, false, "Empty string"},
        {"2024-06-10 12:34:56.1234", {}, false, "Too many msec digits"},
        {"2024-06-10 12:34:56.", {}, false, "Dot but no msec"},
        {"2024-06-10 12:34:56,abc", {}, false, "Comma but non-numeric msec"},
        {"2024-06-10 12:34:56.12a", {}, false, "Dot msec with trailing char"},
#endif
    };

    int numCases = sizeof(testCases) / sizeof(testCases[0]);
    for (int i = 0; i < numCases; ++i) {
        DateTime dt;
        bool success = dt.Parse(testCases[i].str);
        bool ok = (success == testCases[i].expectSuccess);
        if (success && testCases[i].expectSuccess) {
            ok = ok &&
                dt.year  == testCases[i].expected.year &&
                dt.month == testCases[i].expected.month &&
                dt.day   == testCases[i].expected.day &&
                dt.hour  == testCases[i].expected.hour &&
                dt.min   == testCases[i].expected.min &&
                dt.sec   == testCases[i].expected.sec &&
                dt.msec  == testCases[i].expected.msec;
        }
        printf("%-35s | \"%s\" | %s\n",
            testCases[i].description, testCases[i].str, ok ? "OK" : "NG");
        //printf("  Parsed: %d-%02d-%02d %02d:%02d:%02d.%03d\n",
        //    dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, dt.msec);
        //printf("  Expected: %d-%02d-%02d %02d:%02d:%02d.%03d\n",
        //    testCases[i].expected.year, testCases[i].expected.month,
        //    testCases[i].expected.day, testCases[i].expected.hour,
        //    testCases[i].expected.min, testCases[i].expected.sec,
        //    testCases[i].expected.msec);
    }
}

int main()
{
    ::stdio_init_all();

    test_Compare();
    test_CalcDayOfWeek();
    test_Parse();

    for (;;) ::tight_loop_contents();
}
