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

#ifndef _STATUTIL_H_
#define _STATUTIL_H_

//#define MAX_BUCKETS 1000


using namespace std;
namespace silt {
    class DataStat {
    public:
        DataStat(const char* p, double v_min, double v_max,
                 double step_linear,
                 double start_log, double step_log);
        ~DataStat();
        /* insert a new value x */
        void insert(double x);


        size_t index_linear(double x);
        size_t index_log(double x);


        /* return the value of given percentile */
        double percentile(double pct);
        /* return the cumulative probability of value x */
        double cumulative(double x);
        /* return the max value */
        double max() {return x_max;}
        /* return the min value */
        double min() {return x_min;}
        /* return the sum of all values */
        double sum() {return x_sum;}
        /* return the mean value */
        double mean() {return x_sum/x_num;}
        /* return the num of values tracked */
        size_t num()  {return x_num;}
        /* return the median value, i.e. 50% percentile*/
        double median() {return percentile(0.5);}
        /* print a brief summary of this data series */
        void summary();
        /* print the cdf of this data series */
        void cdf(double intv, bool logscale);


    private:
        char name[256];
        double v_min, v_max; // max and min of possible values
        double x_min, x_max; // max and min of input data
        double x_sum;        // sum of all input data
        size_t x_num;      // num of all input data

        size_t num_linear;
        size_t num_log;
        size_t *count_linear;
        size_t *count_log;
        double step_linear;
        double start_log;
        double step_log;
    };
}
#endif /* _STATUTIL_H_ */
