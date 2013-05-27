/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*-
 * SILT: A Memory-Efficient, High-Performance Key-Value Store
 *
 * Copyright © 2011 Carnegie Mellon University
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
 * EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. THIS SOFTWARE IS
 * PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO
 * OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 *
 * GOVERNMENT USE: If you are acquiring this software on behalf of the
 * U.S. government, the Government shall have only "Restricted Rights" in the
 * software and related documentation as defined in the Federal Acquisition
 * Regulations (FARs) in Clause 52.227.19 (c) (2). If you are acquiring the
 * software on behalf of the Department of Defense, the software shall be
 * classified as "Commercial Computer Software" and the Government shall have
 * only "Restricted Rights" as defined in Clause 252.227-7013 (c) (1) of
 * DFARs. Notwithstanding the foregoing, the authors grant the U.S. Government
 * and others acting in its behalf permission to use and distribute the
 * software in accordance with the terms specified in this license.
 */

#include <string>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstring>

#include "datastat.h"
namespace silt {
    DataStat::DataStat(const char* p, double v_min, double v_max,
                       double step_linear = 1.0f,
                       double start_log = 1.0f,
                       double step_log = 10.0f)
        : v_min(v_min), v_max(v_max), x_min(v_max), x_max(v_min),
          x_sum(0.0f), x_num(0), step_linear(step_linear),
          start_log(start_log), step_log(step_log)
    {
        assert(v_min <= v_max);
        assert(step_linear > 0);
        assert(start_log > 0);
        assert(step_log > 1);

        memset(name, 0, 256);
        strncpy(name, p, 256);

        num_linear = (size_t) ceil((v_max - v_min) / step_linear);
        num_log = (size_t) 1 + ceil( log(v_max / start_log) / log(step_log));
        count_linear = new size_t[num_linear + 1];
        count_log = new size_t[num_log + 1];
        memset(count_linear, 0, sizeof(size_t) * num_linear);
        memset(count_log,    0, sizeof(size_t) * num_log);
    }

    DataStat::~DataStat()
    {
    }


    size_t DataStat::index_linear(double x) {
        size_t i;
        i = (size_t) (x - v_min) / step_linear;
        i = ( i > 0 ) ? i : 0;
        i = ( i < num_linear ) ? i : num_linear - 1;
        return i;
    }

    size_t DataStat::index_log(double x) {
        size_t i;
        if (x < start_log) {
            i = 0;
        } else {
            i = (size_t) 1 + log(x / start_log) / log(step_log);
        }
        i = ( i > 0 ) ? i : 0;
        i = ( i < num_log ) ? i : num_log - 1;
        return i;
    }

    void DataStat::insert(double x) {
        x_num ++;
        x_sum += x;
        x_max = (x_max > x ) ? x_max : x;
        x_min = (x_min < x ) ? x_min : x;
        count_linear[index_linear(x)] ++;
        count_log[index_log(x)] ++;
    }

    double DataStat::percentile(double pct) {
        double th = x_num * pct;
        size_t cur;
        double ret_linear  = v_min - 1;
        cur = 0;
        for (size_t i = 0; i < num_linear; i++) {
            cur += count_linear[i];
            if (cur >= th ) {
                ret_linear = v_min + i * step_linear;
                break;
            }
        }

        double ret_log  = v_min - 1;
        cur = 0;
        for (size_t i = 0; i < num_log; i++) {
            cur += count_log[i];
            if (cur >= th ) {
                if ( i == 0 )
                    ret_log = v_min;
                else
                    ret_log = start_log * pow(step_log, i - 1);
                break;
            }
        }

        return std::max(ret_linear, ret_log);
    }

    double DataStat::cumulative(double x) {
        double cum_linear = 0;
        for (size_t i = 0; i < index_linear(x); i++)
            cum_linear += count_linear[i];
        cum_linear = cum_linear * 1.0 / num();

        double cum_log = 0;
        for (size_t i = 0; i < index_log(x); i++)
            cum_log += count_log[i];
        cum_log = cum_log * 1.0 / num();

        return std::max(cum_linear, cum_log);
    }

    void DataStat::summary() {
        printf("========================================\n");
        printf("\tsummary of %s\n", name);
        printf("----------------------------------------\n");
        if (x_num) {
            printf("Number %llu\n", static_cast<unsigned long long>(num()));
            printf("Average %f\n",  mean());
            printf("Median %f\n",   percentile(0.5));
            printf("Min %f\n",      x_min);
            printf("95%%Min %f\n",  percentile(0.05));
            printf("99%%Min %f\n",  percentile(0.01));
            printf("99.9%%Min %f\n",  percentile(0.001));
            printf("Max %f\n",      x_max);
            printf("95%%Max %f\n",  percentile(0.95));
            printf("99%%Max %f\n",  percentile(0.99));
            printf("99.9%%Max %f\n",  percentile(0.999));
        } else
            printf("N/A\n");
        printf("========================================\n");
    }

    void DataStat::cdf(double intv, bool logscale = false) {
        if (logscale)
            assert(intv > 1);
        printf("========================================\n");
        printf("\tCDF of %s\n", name);
        printf("----------------------------------------\n");
        if (num() == 0) {
            printf("N/A\n");
            printf("========================================\n");
            return;
        }

        double x;
        if (logscale) {
            x = start_log;
        }
        else {
            x = v_min;
        }
        while (x < v_max) {
            printf("%f\t%f\n", x, cumulative(x));
            if (logscale)
                x *= intv;
            else
                x += intv;
        }

        printf("========================================\n");
    }
}
