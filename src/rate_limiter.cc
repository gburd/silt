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

#include "rate_limiter.h"
#include "debug.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

namespace silt {

    RateLimiter::RateLimiter(int64_t initial_tokens, int64_t max_tokens, int64_t new_tokens_per_interval, int64_t ns_per_interval, int64_t min_retry_interval_ns)
        : max_tokens_(max_tokens), new_tokens_per_interval_(new_tokens_per_interval), ns_per_interval_(ns_per_interval), min_retry_interval_ns_(min_retry_interval_ns)
    {
        tokens_ = initial_tokens;
#ifndef __APPLE__
        struct timespec ts;

        if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts))
            perror("Error while calling clock_gettime()");

        last_time_ = static_cast<int64_t>(ts.tv_sec) * 1000000000Lu + static_cast<int64_t>(ts.tv_nsec);
#else
        struct timeval tv;

        if (gettimeofday(&tv, NULL))
            perror("Error while calling gettimeofday()");

        last_time_ = static_cast<int64_t>(tv.tv_sec) * 1000000000Lu + static_cast<int64_t>(tv.tv_usec) * 1000Lu;
#endif
    }

    void
    RateLimiter::remove_tokens(int64_t v)
    {
        while (true) {
            update_tokens();
            int64_t extra_tokens_required = v - tokens_;

            if (extra_tokens_required <= 0) {
                // it is OK to make tokens_ negative
                tokens_ -= v;
                return;
            }

            int64_t minimum_ns = extra_tokens_required * ns_per_interval_ / new_tokens_per_interval_;

            if (minimum_ns < min_retry_interval_ns_)
                minimum_ns = min_retry_interval_ns_;

            struct timespec ts;
            ts.tv_sec = minimum_ns / 1000000000L;
            ts.tv_nsec = minimum_ns % 1000000000L;
            //fprintf(stderr, "%Lu %Lu\n", ts.tv_sec, ts.tv_nsec);
            nanosleep(&ts, NULL);
        }
    }

    bool
    RateLimiter::try_remove_tokens(int64_t v)
    {
        update_tokens();
        int64_t extra_tokens_required = v - tokens_;

        if (extra_tokens_required <= 0) {
            // it is OK to make tokens_ negative
            tokens_ -= v;
            return true;
        } else
            return false;
    }

    void
    RateLimiter::update_tokens()
    {
        // TODO: handle time overflow in timespec/timeval
#ifndef __APPLE__
        struct timespec ts;

        if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts))
            perror("Error while calling clock_gettime()");

        int64_t current_time = static_cast<int64_t>(ts.tv_sec) * 1000000000Lu + static_cast<int64_t>(ts.tv_nsec);
#else
        struct timeval tv;

        if (gettimeofday(&tv, NULL))
            perror("Error while calling gettimeofday()");

        int64_t current_time = static_cast<int64_t>(tv.tv_sec) * 1000000000Lu + static_cast<int64_t>(tv.tv_usec) * 1000Lu;
#endif
        int64_t new_intervals = (current_time - last_time_) / ns_per_interval_;

        if (new_intervals < 0) {
            // someone else consumed too much intervals
            return;
        }

        // consume new interval times
        last_time_ += new_intervals * ns_per_interval_;
        int64_t current_tokens = tokens_;
        int64_t new_tokens = new_intervals * new_tokens_per_interval_;

        if (new_tokens + current_tokens > max_tokens_) {
            // trying to avoid exceeding maximum # of tokens
            new_tokens = max_tokens_ - current_tokens;

            if (new_tokens < 0) {
                // the token number is maxed out
                return;
            }
        }

        tokens_ += new_tokens;
    }

} // namespace silt
