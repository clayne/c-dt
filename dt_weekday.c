/*
 * Copyright (c) 2012-2015 Christian Hansen <chansen@cpan.org>
 * <https://github.com/chansen/c-dt>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stddef.h>
#include "dt_core.h"
#include "dt_weekday.h"

dt_t
dt_from_nth_weekday_in_year(int y, int nth) {
    dt_t dt;

    if (nth > 0) {
        dt = dt_add_weekdays(dt_from_yd(y, 0), nth);
        if (nth <= 52 * 5 || dt <= dt_from_yd(y + 1, 0))
            return dt;
    }
    else if (nth < 0) {
        dt = dt_add_weekdays(dt_from_yd(y + 1, 1), nth);
        if (nth >= -52 * 5 || dt >= dt_from_yd(y, 1))
            return dt;
    }
    return 0;
}

dt_t
dt_from_nth_weekday_in_quarter(int y, int q, int nth) {
    dt_t dt;

    if (nth > 0) {
        dt = dt_add_weekdays(dt_from_yqd(y, q, 0), nth);
        if (nth <= 12 * 5 || dt <= dt_from_yqd(y, q + 1, 0))
            return dt;
    }
    else if (nth < 0) {
        dt = dt_add_weekdays(dt_from_yqd(y, q + 1, 1), nth);
        if (nth >= -12 * 5 || dt >= dt_from_yqd(y, q, 1))
            return dt;
    }
    return 0;
}

dt_t
dt_from_nth_weekday_in_month(int y, int m, int nth) {
    dt_t dt;

    if (nth > 0) {
        dt = dt_add_weekdays(dt_from_ymd(y, m, 0), nth);
        if (nth <= 4 * 5 || dt <= dt_from_ymd(y, m + 1, 0))
            return dt;
    }
    else if (nth < 0) {
        dt = dt_add_weekdays(dt_from_ymd(y, m + 1, 1), nth);
        if (nth >= -4 * 5 || dt >= dt_from_ymd(y, m, 1))
            return dt;
    }
    return 0;
}

dt_t
dt_nth_weekday_in_year(dt_t dt, int nth) {
    int y;
    dt_to_yd(dt, &y, NULL);
    return dt_from_nth_weekday_in_year(y, nth);
}

dt_t
dt_nth_weekday_in_quarter(dt_t dt, int nth) {
    int y, q;
    dt_to_yqd(dt, &y, &q, NULL);
    return dt_from_nth_weekday_in_quarter(y, q, nth);
}

dt_t
dt_nth_weekday_in_month(dt_t dt, int nth) {
    int y, m;
    dt_to_ymd(dt, &y, &m, NULL);
    return dt_from_nth_weekday_in_month(y, m, nth);
}

int
dt_weekday_in_year(dt_t dt, bool end) {
    int y;
    dt_t start;

    if (!dt_is_weekday(dt))
        return 0;

    dt_to_yd(dt, &y, NULL);
    if (!end)
        start = dt_from_yd(y, 1);
    else {
        start = dt_from_yd(y + 1, 0);
        if (start == dt)
            return -1;
    }
    return dt_delta_weekdays(start, dt, true);
}

int
dt_weekday_in_quarter(dt_t dt, bool end) {
    int y, q;
    dt_t start;

    if (!dt_is_weekday(dt))
        return 0;

    dt_to_yqd(dt, &y, &q, NULL);
    if (!end)
        start = dt_from_yqd(y, q, 1);
    else {
        start = dt_from_yqd(y, q + 1, 0);
        if (start == dt)
            return -1;
    }
    return dt_delta_weekdays(start, dt, true);
}

int
dt_weekday_in_month(dt_t dt, bool end) {
    int y, m;
    dt_t start;

    if (!dt_is_weekday(dt))
        return 0;

    dt_to_ymd(dt, &y, &m, NULL);
    if (!end)
        start = dt_from_ymd(y, m, 1);
    else {
        start = dt_from_ymd(y, m + 1, 0);
        if (start == dt)
            return -1;
    }
    return dt_delta_weekdays(start, dt, true);
}

bool
dt_is_weekday(dt_t dt) {
    return (dt_dow(dt) <= DT_FRIDAY);
}

dt_t
dt_next_weekday(dt_t dt, bool current) {
    static const int T[2][8] = {
        /*  M T W T F S S */
        { 0,1,1,1,1,3,2,1 },
        { 0,0,0,0,0,0,2,1 },
    };
    return dt + T[!!current][dt_dow(dt)];
}

dt_t
dt_prev_weekday(dt_t dt, bool current) {
    static const int T[2][8] = {
        /*  M T W T F S S */
        { 0,3,1,1,1,1,1,2 },
        { 0,0,0,0,0,0,1,2 },
    };
    return dt - T[!!current][dt_dow(dt)];
}

dt_t
dt_add_weekdays(dt_t dt, int delta) {
    int w, d;

    if (!delta)
        return dt;

    w = delta / 5;
    d = delta - 5 * w;

    if (delta > 0) {
        static const int T[35] = {
            /*       0 1 2 3 4  (+n days) */
            /* M */  0,1,2,3,4,
            /* T */  0,1,2,3,6,
            /* W */  0,1,2,5,6,
            /* T */  0,1,4,5,6,
            /* F */  0,3,4,5,6,
            /* S */ -1,2,3,4,5,
            /* S */ -2,1,2,3,4,
        };
        return dt + 7 * w + T[5 * dt_dow(dt) - 5 + d];
    }
    else {
        static const int T[35] = {
            /*       0 1 2 3 4  (-n days) */
            /* M */  0,3,4,5,6,
            /* T */  0,1,4,5,6,
            /* W */  0,1,2,5,6,
            /* T */  0,1,2,3,6,
            /* F */  0,1,2,3,4,
            /* S */ -2,1,2,3,4,
            /* S */ -1,2,3,4,5,
        };
        return dt + 7 * w - T[5 * dt_dow(dt) - 5 - d];
    }
}

int
dt_delta_weekdays(dt_t dt1, dt_t dt2, bool inclusive) {
    static const int T[98] = {
        /*      M T W T F S S  (exclusive) */
        /* M */ 0,1,2,3,4,4,4,
        /* T */ 4,0,1,2,3,3,3,
        /* W */ 3,4,0,1,2,2,2,
        /* T */ 2,3,4,0,1,1,1,
        /* F */ 1,2,3,4,0,0,0,
        /* S */ 0,1,2,3,4,0,0,
        /* S */ 0,1,2,3,4,5,0,
        /*      M T W T F S S  (inclusive) */
        /* M */ 1,2,3,4,5,5,5,
        /* T */ 5,1,2,3,4,4,4,
        /* W */ 4,5,1,2,3,3,3,
        /* T */ 3,4,5,1,2,2,2,
        /* F */ 2,3,4,5,1,1,1,
        /* S */ 1,2,3,4,5,0,0,
        /* S */ 1,2,3,4,5,5,0,
    };
    const int d1 = dt_dow(dt1);
    const int d2 = dt_dow(dt2);

    if (dt1 <= dt2)
        return 5 * ((dt2 - dt1) / 7) + T[7 * d1 + d2 - 8 + (49 * !!inclusive)];
    else
        return 5 * ((dt2 - dt1) / 7) - T[7 * d2 + d1 - 8 + (49 * !!inclusive)];
}

