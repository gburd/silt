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

#include "global_limits.h"

#include <cstdio>

namespace silt {

    GlobalLimits GlobalLimits::global_limits_;

    GlobalLimits::GlobalLimits()
        : convert_rate_limiter_(0, 1000000000L, 1, 1),
          merge_rate_limiter_(0, 1000000000L, 1, 1)
    {
        disabled_ = 0;
        set_convert_rate(1000000000L);
        set_merge_rate(1000000000L);
    }

    GlobalLimits::~GlobalLimits()
    {
    }

    void
    GlobalLimits::set_convert_rate(int64_t v)
    {
        convert_rate_limiter_.set_tokens(v / 100);
        convert_rate_limiter_.set_max_tokens(v / 100);    // allows burst of 0.01 sec
        convert_rate_limiter_.set_ns_per_interval(1000000000L / v);
    }

    void
    GlobalLimits::set_merge_rate(int64_t v)
    {
        merge_rate_limiter_.set_tokens(v / 100);
        merge_rate_limiter_.set_max_tokens(v / 100);    // allows burst of 0.01 sec
        merge_rate_limiter_.set_ns_per_interval(1000000000L / v);
    }

    void
    GlobalLimits::remove_convert_tokens(int64_t v)
    {
        if (enabled()) {
            convert_rate_limiter_.remove_tokens(v);
            // TODO: DPRINTF/fprintf(stderr, "convert tokens: %lld\n", convert_rate_limiter_.tokens());
        }
    }

    void
    GlobalLimits::remove_merge_tokens(int64_t v)
    {
        if (enabled()) {
            merge_rate_limiter_.remove_tokens(v);
            // TODO: DPRINTF/fprintf(stderr, "merge tokens: %lld\n", merge_rate_limiter_.tokens());
        }
    }

    void
    GlobalLimits::enable()
    {
        if (--disabled_ < 0) {
            ++disabled_;
            fprintf(stderr, "warning: probably too many calls to GlobalLimits::enable()\n");
        }
    }

    void
    GlobalLimits::disable()
    {
        ++disabled_;
    }

} // namespace silt
