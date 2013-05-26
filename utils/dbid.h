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

#ifndef _DBID_H_
#define _DBID_H_

using namespace std;
#include <iostream>
#include <cstdlib>
#include <stdint.h>

/**
 * DBID is a fixed-length identifier for nodes on a ringspace.  Since
 * it is fixed length, the length needs to be as long as the longest
 * allowable key size so that the ID space contains the granularity to
 * load balance data.  This length is defined as DBID_LENGTH below.
 *
 * In other words: DBID_LENGTH is the maximum key length for any query.
 *
 * Comparisons of short keys will pad the incoming key with 0s to
 * meet this size when necessary.  You can redefine DBID_LENGTH to
 * whatever size you desire, but DBID comparisons will take time
 * proportional to the size.
 **/

namespace silt {
    const uint32_t DBID_LENGTH = 1024;

    class DBID
    {

    private:
        char value[DBID_LENGTH];
        unsigned int actual_size;
    public:
        DBID();
        DBID(const char *c, unsigned int size);
        explicit DBID(const string &s);
        ~DBID();
        void Init(const char *c, unsigned int size);

        char* data() {
            return value;
        }
        const char *const_data() const {
            return (const char *)value;
        }
        string* actual_data();
        const string actual_data_str() const;
        uint32_t size() {
            return DBID_LENGTH;
        }
        unsigned int get_actual_size() {
            return actual_size;
        }
        void printValue() const;
        void printValue(int prefix_bytes) const;
        void set_actual_size(unsigned int as) {
            actual_size = as;
        }

        bool operator==(const DBID &rhs) const;
        bool operator<(const DBID &rhs) const;

        // This compares only the top 64 bits of each DBID
        uint64_t operator-(const DBID &rhs) const;

        //DBID operator-(const DBID &rhs) const;
    };

    bool between(const string& startKey, const string& endKey, const string& thisKey);
    bool between(const DBID* startID, const DBID* endID, const DBID* thisID);
}
#endif
