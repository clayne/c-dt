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
#include "dt_core.h"
#include "dt_util.h"
#include "dt_arithmetic.h"

dt_t
dt_add_years(dt_t dt, int delta, dt_adjust_t adjust) {
    int y, d;

    dt_to_yd(dt, &y, &d);
    if (adjust == DT_EXCESS || d < 365)
        return dt_from_yd(y + delta, d);
    else {
        int ry = y + delta;
        int diy;

        diy = dt_days_in_year(ry);
        if (d > diy || (adjust == DT_SNAP && d == dt_days_in_year(y)))
            d = diy;
        return dt_from_yd(ry, d);
    }
}

dt_t
dt_add_quarters(dt_t dt, int delta, dt_adjust_t adjust) {
    int y, q, d;

    dt_to_yqd(dt, &y, &q, &d);
    if (adjust == DT_EXCESS || d < 90)
        return dt_from_yqd(y, q + delta, d);
    else {
        int ry = y;
        int rq = q + delta;
        int diq;

        if (rq < 1 || rq > 4) {
            ry += rq / 4;
            rq %= 4;
            if (rq < 1)
                ry--, rq += 4;
        }

        diq = dt_days_in_quarter(ry, rq);
        if (d > diq || (adjust == DT_SNAP && d == dt_days_in_quarter(y, q)))
            d = diq;
        return dt_from_yqd(ry, rq, d);
    }
}

dt_t
dt_add_months(dt_t dt, int delta, dt_adjust_t adjust) {
    int y, m, d;

    dt_to_ymd(dt, &y, &m, &d);
    if (adjust == DT_EXCESS || d < 28)
        return dt_from_ymd(y, m + delta, d);
    else {
        int ry = y;
        int rm = m + delta;
        int dim;

        if (rm < 1 || rm > 12) {
            ry += rm / 12;
            rm %= 12;
            if (rm < 1)
                ry--, rm += 12;
        }

        dim = dt_days_in_month(ry, rm);
        if (d > dim || (adjust == DT_SNAP && d == dt_days_in_month(y, m)))
            d = dim;
        return dt_from_ymd(ry, rm, d);
    }
}

void
dt_delta_yd(dt_t dt1, dt_t dt2, int *yp, int *dp) {
    int y1, y2, d1, d2, years, days;

    dt_to_yd(dt1, &y1, &d1);
    dt_to_yd(dt2, &y2, &d2);

    years = y2 - y1;
    days = d2 - d1;

    if (years > 0 && days < 0) {
        years--;
        days = dt2 - dt_add_years(dt1, years, DT_LIMIT);
    }
    else if (years < 0 && days > 0) {
        years++;
        days -= dt_days_in_year(y2);
    }
    
    if (yp) *yp = years;
    if (dp) *dp = days;
}

void
dt_delta_yqd(dt_t dt1, dt_t dt2, int *yp, int *qp, int *dp) {
    int y1, y2, q1, q2, d1, d2, years, quarters, days;

    dt_to_yqd(dt1, &y1, &q1, &d1);
    dt_to_yqd(dt2, &y2, &q2, &d2);

    quarters = 4 * (y2 - y1) + q2 - q1;
    days = d2 - d1;

    if (quarters > 0 && days < 0) {
        quarters--;
        days = dt2 - dt_add_quarters(dt1, quarters, DT_LIMIT);
    }
    else if (quarters < 0 && days > 0) {
        quarters++;
        days -= dt_days_in_quarter(y2, q2);
    }
    
    years = quarters / 4;
    quarters = quarters - years * 4;
    
    if (qp) *yp = years;
    if (qp) *qp = quarters;
    if (dp) *dp = days;
}

void
dt_delta_ymd(dt_t dt1, dt_t dt2, int *yp, int *mp, int *dp) {
    int y1, y2, m1, m2, d1, d2, years, months, days;

    dt_to_ymd(dt1, &y1, &m1, &d1);
    dt_to_ymd(dt2, &y2, &m2, &d2);

    months = 12 * (y2 - y1) + m2 - m1;
    days = d2 - d1;

    if (months > 0 && days < 0) {
        months--;
        days = dt2 - dt_add_months(dt1, months, DT_LIMIT);
    }
    else if (months < 0 && days > 0) {
        months++;
        days -= dt_days_in_month(y2, m2);
    }

    years = months / 12;
    months = months - years * 12;

    if (yp) *yp = years;
    if (mp) *mp = months;
    if (dp) *dp = days;
}

int
dt_delta_years(dt_t dt1, dt_t dt2, bool complete) {
    int y1, y2, d1, d2, years;

    dt_to_yd(dt1, &y1, &d1);
    dt_to_yd(dt2, &y2, &d2);

    years = y2 - y1;
    if (complete) {
        if (dt1 > dt2)
            years += (d2 > d1);
        else
            years -= (d1 > d2);
    }
    return years;
}

int
dt_delta_quarters(dt_t dt1, dt_t dt2, bool complete) {
    int y1, y2, q1, q2, d1, d2, quarters;

    dt_to_yqd(dt1, &y1, &q1, &d1);
    dt_to_yqd(dt2, &y2, &q2, &d2);

    quarters = 4 * (y2 - y1) + q2 - q1;
    if (complete) {
        if (dt1 > dt2)
            quarters += (d2 > d1);
        else
            quarters -= (d1 > d2);
    }
    return quarters;
}

int
dt_delta_months(dt_t dt1, dt_t dt2, bool complete) {
    int y1, y2, m1, m2, d1, d2, months;

    dt_to_ymd(dt1, &y1, &m1, &d1);
    dt_to_ymd(dt2, &y2, &m2, &d2);

    months = 12 * (y2 - y1) + m2 - m1;
    if (complete) {
        if (dt1 > dt2)
            months += (d2 > d1);
        else
            months -= (d1 > d2);
    }
    return months;
}

int
dt_delta_weeks(dt_t dt1, dt_t dt2) {
    return (dt2 - dt1) / 7;
}

