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

#include "hash_functions.h"
#include "hashutil.h"

namespace silt {

    hashfn_t Hashes::hashes[HASH_COUNT] = { &Hashes::h1,
                                            &Hashes::h2,
                                            &Hashes::h3 };

    uint32_t Hashes::h1(const void* buf, size_t len)
    {
        return HashUtil::BobHash(buf, len, 0xdeadbeef);
    }
    uint32_t Hashes::h2(const void* buf, size_t len)
    {
        return HashUtil::BobHash(buf, len, 0xc0ffee);
    }
    uint32_t Hashes::h3(const void* buf, size_t len)
    {
        return HashUtil::BobHash(buf, len, 0x101CA75);
    }

    // These hashes assume that len > 32bits
    uint32_t Hashes::nullhash1(const void* buf, size_t len)
    {
        if (len < 4) printf("Warning: keylength is not long enough to use null hash 1.  Ensure keylength >= 32bits\n");
        return HashUtil::NullHash(buf, len, 0);
    }

    uint32_t Hashes::nullhash2(const void* buf, size_t len)
    {
        if (len < 8) printf("Warning: keylength is not long enough to use null hash 2.  Ensure keylength >= 64bits\n");
        return HashUtil::NullHash(buf, len, 4);
    }

    uint32_t Hashes::nullhash3(const void* buf, size_t len)
    {
        if (len < 12) printf("Warning: keylength is not long enough to use null hash 3.  Ensure keylength >= 96bits\n");
        return HashUtil::NullHash(buf, len, 8);
    }

}  // namespace silt
