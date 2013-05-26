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

#ifndef _SILT_COMBI_H_
#define _SILT_COMBI_H_

#include "silt.h"
#include "task.h"
#include <tbb/queuing_rw_mutex.h>

namespace silt {

    // configuration
    //   <type>: "combi" (fixed)
    //   <id>: the ID of the store
    //   <file>: the file name prefix to store bookkeeping information for persistency
    //   <key-len>: the length of keys -- zero for variable-length keys (default), a positive integer for fixed-length keys
    //   <data-len>: the length of data -- zero for variable-length data (default), a positive integer for fixed-length data
    //   <temp-file>: the path of the temporary files/directory. "/tmp" is the default.
    //   <stage-limit>: limit the use of stages up to the specific number (e.g. 1: no store2 will be used); will still need to specify the configuration for all stores.  2 for no limit (default)
    //   <store0-high-watermark>: the number of front stores to begin conversion to a middle store.  2 is default.
    //   <store0-low-watermark>: the number of front stores to stop conversion to a middle store.  1 is default.  Must be at least 1 due to the writable store. store. store. store.
    //   <store1-high-watermark>: the number of middle stores to begin merge into the back store.  1 is default.
    //   <store1-low-watermark>: the number of middle stores to stop merge into the back store.  0 is default.
    //   <store0>: the configuration for front stores
    //             <id>: will be assigned by Silt_Combi
    //             <key-len>: will be set by Silt_Combi
    //             <data-len>: will be set by Silt_Combi
    //   <store1>: the configuration for middle stores
    //             <id>: will be assigned by Silt_Combi
    //             <key-len>: will be set by Silt_Combi
    //             <data-len>: will be set by Silt_Combi
    //   <store2>: the configuration for back stores
    //             <id>: will be assigned by Silt_Combi
    //             <key-len>: will be set by Silt_Combi
    //             <data-len>: will be set by Silt_Combi
    //             <size>: will be set by Silt_Combi

    class Silt_Combi : public Silt {
    public:
        Silt_Combi();
        virtual ~Silt_Combi();

        virtual Silt_Return Create();
        virtual Silt_Return Open();

        virtual Silt_Return Flush();
        virtual Silt_Return Close();

        virtual Silt_Return Destroy();

        virtual Silt_Return Status(const Silt_StatusType& type, Value& status) const;

        virtual Silt_Return Put(const ConstValue& key, const ConstValue& data);
        virtual Silt_Return Append(Value& key, const ConstValue& data);

        virtual Silt_Return Delete(const ConstValue& key);

        virtual Silt_Return Contains(const ConstValue& key) const;
        virtual Silt_Return Length(const ConstValue& key, size_t& len) const;
        virtual Silt_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;

        virtual Silt_ConstIterator Enumerate() const;
        virtual Silt_Iterator Enumerate();

        //virtual Silt_ConstIterator Find(const ConstValue& key) const;
        //virtual Silt_Iterator Find(const ConstValue& key);

        struct IteratorElem : public Silt_IteratorElem {
            IteratorElem(const Silt_Combi* silt);
            ~IteratorElem();

            Silt_IteratorElem* Clone() const;
            void Next();

            tbb::queuing_rw_mutex::scoped_lock* lock;
            size_t current_stage;
            size_t current_store;
            Silt_Iterator store_it;
        };

    protected:
        Silt* alloc_store(size_t stage, size_t size = -1);

        class ConvertTask : public Task {
        public:
            virtual void Run();
            Silt_Combi* silt;

            Silt* convert(Silt* front_store);
        };

        class MergeTask : public Task {
        public:
            virtual void Run();
            Silt_Combi* silt;

            Silt* sort(Silt* sorter, Silt* middle_store, size_t& num_adds, size_t& num_dels);
            Silt* merge(Silt* back_store, Silt* sorter, size_t& num_adds, size_t& num_dels);
        };

    private:
        bool open_;

        std::string id_;

        size_t key_len_;
        size_t data_len_;

        std::string temp_file_;

        size_t stage_limit_;

        size_t store0_high_watermark_;
        size_t store0_low_watermark_;

        size_t store1_high_watermark_;
        size_t store1_low_watermark_;

        mutable tbb::queuing_rw_mutex mutex_;

        std::vector<std::vector<Silt*> > all_stores_;     // protected by mutex_
        std::vector<size_t> next_ids_;                    // protected by mutex_

        size_t back_store_size_;

        tbb::atomic<bool> convert_task_running_;    // also protected by mutex_
        tbb::atomic<bool> merge_task_running_;      // also protected by mutex_

        friend class MergeTask;

        static TaskScheduler task_scheduler_convert_;
        static TaskScheduler task_scheduler_merge_;

        static const size_t latency_track_store_count_ = 100;
        mutable tbb::atomic<uint64_t> latencies_[4][latency_track_store_count_];
        mutable tbb::atomic<uint64_t> counts_[4][latency_track_store_count_];
    };

} // namespace silt

#endif  // #ifndef _SILT_COMBI_H_
