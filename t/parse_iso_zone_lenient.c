#include "dt.h"
#include "tap.h"
#include <string.h>

const struct good_t {
    int offset;
    const char *str;
    size_t elen;
} good[] = {
    {     0,   "Z",      1 },
    {     0,   "z",      1 },
    {     0,   "GMT",    3 },
    {   180,   "GMT+03", 6 },
    {  -180,   "GMT-03", 6 },
    {   180,   "GMT+3",  5 },
    {  -180,   "GMT-3",  5 },
    {     0,   "UTC",    3 },
    {   180,   "UTC+03", 6 },
    {  -180,   "UTC-03", 6 },
    {   180,   "UTC+3",  5 },
    {  -180,   "UTC-3",  5 },
    {     0,   "+0",     2 },
    {     0,   "-0",     2 },
    {    60,   "+1",     2 },
    {   -60,   "-1",     2 },
    {    60,   "+1:00",  5 },
    {   -60,   "-1:00",  5 },
    {     0,   "+00",    3 },
    {  1380,   "+23",    3 },
    {    60,   "+01",    3 },
    { -1380,   "-23",    3 },
    {   -60,   "-01",    3 },
    {     0,   "+00:00", 6 },
    {    59,   "+00:59", 6 },
    {   -59,   "-00:59", 6 },
    {  1439,   "+23:59", 6 },
    { -1439,   "-23:59", 6 },
    {     0,   "+0000",  5 },
    {    59,   "+0059",  5 },
    {   -59,   "-0059",  5 },
    {  1439,   "+2359",  5 },
    { -1439,   "-2359",  5 },
};

const struct bad_t {
    const char *str;
} bad[] = {
    { ""        },
    { "0"       },
    { "+00:0"   },
    { "-00:0"   },
    { "+00:60"  },
    { "-00:60"  },
    { "+24:00"  },
    { "+00:000" },
    { "-00:000" },
    { "+0060"   },
    { "-0060"   },
    { "-0060"   },
    { "+2400"   },
    { "+00000"  },
    { "-00000"  },
    { "UT"      },
    { "UTc"     },
    { "UTC+"    },
    { "UTC-"    },
    { "GM"      },
    { "GMt"     },
    { "GMT+"    },
    { "GMT-"    },
};

int 
main() {
    int i, ntests;

    ntests = sizeof(good) / sizeof(*good);
    for (i = 0; i < ntests; i++) {
        const struct good_t t = good[i];

        {
            int offset = 0;
            size_t glen;

            glen = dt_parse_iso_zone_lenient(t.str, strlen(t.str), &offset);
            ok(glen == t.elen, "dt_parse_iso_zone_lenient(%s) size_t: %d", t.str, (int)glen);
            cmp_ok(offset, "==", t.offset, "dt_parse_iso_zone_lenient(%s)", t.str);
        }
    }

    ntests = sizeof(bad) / sizeof(*bad);
    for (i = 0; i < ntests; i++) {
        const struct bad_t t = bad[i];

        {
            int offset = 0;
            size_t glen;

            glen = dt_parse_iso_zone_lenient(t.str, strlen(t.str), &offset);
            ok(glen == 0, "dt_parse_iso_zone_lenient(%s) size_t: %d", t.str, (int)glen);
        }
    }
    done_testing();
}

