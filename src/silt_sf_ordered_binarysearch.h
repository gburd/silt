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

#ifndef _SILT_SF_ORDERED_BINARYSEARCH_H_
#define _SILT_SF_ORDERED_BINARYSEARCH_H_

#include "silt_factory.h"

namespace silt {
    template <typename Store>
    class Silt_SF_Ordered_BinarySearch : public Silt {
        public:
            static Silt_SF_Ordered_BinarySearch<Store> *Create(Configuration *const &config, const uint64_t &hash_table_size) __attribute__((warn_unused_result));
            static Silt_SF_Ordered_BinarySearch<Store> *Open(Configuration *const &config) __attribute__((warn_unused_result));

        protected:
            Silt_SF_Ordered_BinarySearch(Configuration *const &config);
            void Initialize(uint64_t hash_table_size);
            void LoadFromFile();
            void StoreToFile();

        public:
            ~Silt_SF_Ordered_BinarySearch();

            int Insert(const char *const &key, const uint32_t &key_len, const char *const &data, const uint32_t &data_len) __attribute__((warn_unused_result));
            int Get(const char *const &key, const uint32_t &key_len, char *&data, uint32_t &data_len) __attribute__((warn_unused_result));
            int Delete(const char *const &key, const uint32_t &key_len) __attribute__((warn_unused_result));
            //int DoForEach(void (*callForEach)(const uint32_t &key_len, const uint32_t &data_len, const char* const &key, const char* const &data, const bool &isDeleted), void (*callBeforeRest)()) const __attribute__ ((warn_unused_result));
            int Flush() __attribute__((warn_unused_result));
            int Destroy() __attribute__((warn_unused_result));

            SiltIter IteratorBegin() const __attribute__((warn_unused_result));

        protected:
            string id_;
            Configuration *config_;

            Store *data_store_;

        protected:
            std::size_t lookup_key_index(const char *key, uint64_t off, int64_t n) const;

            void finalize();

            std::size_t hash_state() const;

        private:
            bool initialized_;
            bool finalized_;

            uint32_t key_len_;
            uint32_t data_len_;

            off_t base_data_store_offset_;
            uint64_t hash_table_size_;
            uint64_t actual_size_;

            char *last_inserted_key_;   // for validating new keys
    };
}  // namespace silt

#endif  // #ifndef _SILT_SF_ORDERED_BINARYSEARCH_H_
