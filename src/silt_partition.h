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

#ifndef _SILT_PARTITION_H_
#define _SILT_PARTITION_H_

#include "silt.h"
#include <vector>

namespace silt {

    // configuration
    //   <type>: "partition" (fixed)
    //   <id>: the ID of the store
    //   <skip-bits>: the number of MSBs to ignore when calculating the partition number
    //   <partitions>: the number of partitions; must be power of 2
    //   <store>: the configuration for the lower-level store
    //            <id>: will be assigned by Silt_Partition

    class Silt_Partition : public Silt {
        public:
            Silt_Partition();
            virtual ~Silt_Partition();

            virtual Silt_Return Create();
            virtual Silt_Return Open();

            virtual Silt_Return Flush();
            virtual Silt_Return Close();

            virtual Silt_Return Destroy();

            virtual Silt_Return Status(const Silt_StatusType &type, Value &status) const;

            virtual Silt_Return Put(const ConstValue &key, const ConstValue &data);
            virtual Silt_Return Append(Value &key, const ConstValue &data);

            virtual Silt_Return Delete(const ConstValue &key);

            virtual Silt_Return Contains(const ConstValue &key) const;
            virtual Silt_Return Length(const ConstValue &key, size_t &len) const;
            virtual Silt_Return Get(const ConstValue &key, Value &data, size_t offset = 0, size_t len = -1) const;

            virtual Silt_ConstIterator Enumerate() const;
            virtual Silt_Iterator Enumerate();

            virtual Silt_ConstIterator Find(const ConstValue &key) const;
            virtual Silt_Iterator Find(const ConstValue &key);

            struct IteratorElem : public Silt_IteratorElem {
                IteratorElem(const Silt_Partition *silt);
                ~IteratorElem();

                Silt_IteratorElem *Clone() const;
                void Next();

                size_t next_store;
                Silt_Iterator store_it;
            };

        private:
            Silt_Return alloc_stores();
            size_t get_partition(const ConstValue &key) const;

            size_t skip_bits_;
            size_t partitions_;
            size_t partition_bits_;

            std::vector<Silt *> stores_;

            friend struct IteratorElem;
    };

} // namespace silt

#endif  // #ifndef _SILT_PARTITION_H_
