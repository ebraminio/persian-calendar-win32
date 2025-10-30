// https://github.com/roozbehp/persiancalendar/blob/main/persiancalendar_fast.py
// This code is partially based on the Common Lisp code published by
// Reingold and Dershowitz under the Apache 2.0 license.
//
// Python port and modifications for using the 33-year cycle were
// made by Roozbeh Pournader.
//
// Copyright 2024 Roozbeh Pournader
//
// The original header follows:
//
// CALENDRICA 4.0 -- Common Lisp
// E. M. Reingold and N. Dershowitz
//
// ================================================================
//
// The Functions (code, comments, and definitions) contained in this
// file (the "Program") were written by Edward M. Reingold and Nachum
// Dershowitz (the "Authors"), who retain all rights to them except as
// granted in the License and subject to the warranty and liability
// limitations listed therein.  These Functions are explained in the Authors'
// book, "Calendrical Calculations", 4th ed. (Cambridge University
// Press, 2016), and are subject to an international copyright.
//
// Licensed under the Apache License, Version 2.0 <LICENSE or
// https://www.apache.org/licenses/LICENSE-2.0>.
//
// Sample values for the functions (useful for debugging) are given in
// Appendix C of the book.

#include <stdbool.h>

#define PERSIAN_EPOCH 226896 // Precalculated result from Calendrical Calculations

#define SUPPORTED_FIRST_YEAR 1178
#define SUPPORTED_LAST_YEAR 3000

// Non-leap years that would be considered leap by the 33-year rule
const int NON_LEAP_CORRECTION[] = {
    1502,
    1601, 1634, 1667,
    1700, 1733, 1766, 1799,
    1832, 1865, 1898,
    1931, 1964, 1997,
    2030, 2059, 2063, 2096,
    2129, 2158, 2162, 2191, 2195,
    2224, 2228, 2257, 2261, 2290, 2294,
    2323, 2327, 2356, 2360, 2389, 2393,
    2422, 2426, 2455, 2459, 2488, 2492,
    2521, 2525, 2554, 2558, 2587, 2591,
    2620, 2624, 2653, 2657, 2686, 2690,
    2719, 2723, 2748, 2752, 2756, 2781, 2785, 2789,
    2818, 2822, 2847, 2851, 2855, 2880, 2884, 2888,
    2913, 2917, 2921, 2946, 2950, 2954, 2979, 2983, 2987};

typedef struct
{
    int year;
    int month;
    int day;
} PersianDate;

bool is_in_non_leap_correction(int year)
{
    if (year == 0)
        return false;
    for (int i = 0; i < sizeof(NON_LEAP_CORRECTION) / sizeof(int); ++i)
        if (NON_LEAP_CORRECTION[i] == year)
            return true;
    return false;
}

int div_ceil(int a, int b)
{
    // return __builtin_ceil((double)a / b);
    float v = (float)a / b;
    return (float)(int)v == v ? v : v + 1;
}

int fixed_from_persian_fast(PersianDate p_date)
{
    int year = p_date.year;
    int month = p_date.month;
    int day = p_date.day;

    int new_year = PERSIAN_EPOCH - 1 + 365 * (year - 1) + (8 * year + 21) / 33;

    if (is_in_non_leap_correction(year - 1))
    {
        new_year -= 1;
    }

    // Days in prior months this year
    int days_in_prior_months;
    if (month <= 7)
        days_in_prior_months = 31 * (month - 1);
    else
        days_in_prior_months = 30 * (month - 1) + 6;

    return (new_year - 1)         // Days in prior years
           + days_in_prior_months // Days in prior months this year
           + day;                 // Days so far this month
}

PersianDate persian_fast_from_fixed(int date)
{
    // Calculate first day of year 1
    PersianDate first_day = {1, 1, 1};
    int first_fixed = fixed_from_persian_fast(first_day);

    int days_since_epoch = date - first_fixed;
    int year = 1 + (33 * days_since_epoch + 3) / 12053;

    // Calculate day of year
    PersianDate new_year_day = {year, 1, 1};
    int day_of_year = date - fixed_from_persian_fast(new_year_day) + 1;

    // Handle leap year correction
    if (day_of_year == 366 && is_in_non_leap_correction(year))
    {
        year += 1;
        day_of_year = 1;
    }

    // Calculate month
    int month;
    if (day_of_year <= 186)
        month = div_ceil(day_of_year, 31);
    else
        month = div_ceil(day_of_year - 6, 30);

    // Calculate day by subtraction
    PersianDate month_start = {year, month, 1};
    int day = date - fixed_from_persian_fast(month_start) + 1;

    return (PersianDate){
        .year = year,
        .month = month,
        .day = day,
    };
}

bool persian_fast_leap_year(int p_year)
{
    if (is_in_non_leap_correction(p_year))
        return false;
    else if (is_in_non_leap_correction(p_year - 1))
        return true;
    else
        return (25 * p_year + 11) % 33 < 8;
}
