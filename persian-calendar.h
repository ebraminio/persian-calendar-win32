// Modified from https://github.com/SCR-IR/jalaliDate-Cpp/blob/e3f5989/src/converter.cpp
// Have a look at the source for more reliable implementation as that
// is changed here for better or worse.

/**  Gregorian & Jalali (Hijri_Shamsi,Solar) Date Converter Functions
Author: JDF.SCR.IR =>> Download Full Version :  http://jdf.scr.ir/jdf
License: GNU/LGPL _ Open Source & Free :: Version: 2.80 : [2020=1399]
---------------------------------------------------------------------
355746=361590-5844 & 361590=(30*33*365)+(30*8) & 5844=(16*365)+(16/4)
355666=355746-79-1 & 355668=355746-79+1 &  1595=605+990 &  605=621-16
990=30*33 & 12053=(365*33)+(32/4) & 36524=(365*100)+(100/4)-(100/100)
1461=(365*4)+(4/4) & 146097=(365*400)+(400/4)-(400/100)+(400/400)  */

#include <stdint.h>
inline void jdn_to_persian(uint32_t jdn, uint32_t *py, uint32_t *pm, uint32_t *pd) {
  uint32_t year = jdn / 12053 * 33 - 1595;
  jdn %= 12053;
  year += jdn / 1461 * 4;
  jdn %= 1461;
  if (jdn > 365) {
    year += (jdn - 1) / 365;
    jdn = (jdn - 1) % 365;
  }
  *py = year;
  if (jdn < 186) {
    *pm = 1 + jdn / 31;
    *pd = 1 + jdn % 31;
  } else {
    *pm = 7 + (jdn - 186) / 30;
    *pd = 1 + (jdn - 186) % 30;
  }
}
