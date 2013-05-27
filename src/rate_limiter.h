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

#ifndef _RATE_LIMITER_H_
#define _RATE_LIMITER_H_

#include <tbb/atomic.h>

namespace silt {

    // a high-performance high-precision rate limiter based on token bucket

    class RateLimiter {
        public:
            RateLimiter(int64_t initial_tokens, int64_t max_tokens, int64_t new_tokens_per_interval, int64_t ns_per_interval, int64_t min_retry_interval_ns = 1000L);

            void remove_tokens(int64_t v);
            bool try_remove_tokens(int64_t v);

            void remove_tokens_nowait(int64_t v) {
                tokens_ -= v;
            }

            void add_tokens(int64_t v) {
                tokens_ += v;
            }

            int64_t tokens() const {
                return tokens_;
            }
            void set_tokens(int64_t v) {
                tokens_ = v;
            }

            const int64_t &max_tokens() const {
                return max_tokens_;
            }
            void set_max_tokens(int64_t v) {
                max_tokens_ = v;
            }

            const int64_t &new_tokens_per_interval() const {
                return new_tokens_per_interval_;
            }
            void set_new_tokens_per_interval(int64_t v) {
                new_tokens_per_interval_ = v;
            }

            const int64_t &ns_per_interval() const {
                return ns_per_interval_;
            }
            void set_ns_per_interval(int64_t v) {
                ns_per_interval_ = v;
            }

            const int64_t &min_retry_interval_ns() const {
                return min_retry_interval_ns_;
            }
            void set_min_retry_interval_ns(int64_t v) {
                min_retry_interval_ns_ = v;
            }

        protected:
            void update_tokens();

        private:
            tbb::atomic<int64_t> tokens_;
            int64_t max_tokens_;
            int64_t new_tokens_per_interval_;
            int64_t ns_per_interval_;
            int64_t min_retry_interval_ns_;

            tbb::atomic<int64_t> last_time_;
    };

} // namespace silt

#endif  // #ifndef _RATE_LIMITER_H_
