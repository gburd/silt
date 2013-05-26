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

#ifndef _FILE_STORE_H_
#define _FILE_STORE_H_

#include "silt.h"
#include "file_io.h"    // for iovec
#include "task.h"
#include <tbb/atomic.h>
#include <tbb/queuing_mutex.h>
#include <tbb/queuing_rw_mutex.h>
#include <boost/dynamic_bitset.hpp>

namespace silt {

    // configuration
    //   <type>: "file" (fixed)
    //   <id>: the ID of the file store
    //   <file>: the file name prefix to store log entries for persistence
    //   <data-len>: the length of data -- zero for variable-length data (default), a positive integer for fixed-length data (space optimization is applied)
    //   <use-buffered-io-only>: with a non-zero value, use buffered I/O only and do not use direct I/O.  Default is 0 (false).  Useful for quick tests or data-len >= 4096 (direct I/O is less likely to improve read performance).

    class FileStore : public Silt {
    public:
        FileStore();
        virtual ~FileStore();

        virtual Silt_Return Create();
        virtual Silt_Return Open();

        virtual Silt_Return ConvertTo(Silt* new_store) const;

        virtual Silt_Return Flush();
        virtual Silt_Return Close();

        virtual Silt_Return Destroy();

        virtual Silt_Return Status(const Silt_StatusType& type, Value& status) const;

        virtual Silt_Return Put(const ConstValue& key, const ConstValue& data);
        virtual Silt_Return Append(Value& key, const ConstValue& data);

        //virtual Silt_Return Delete(const ConstValue& key);

        virtual Silt_Return Contains(const ConstValue& key) const;
        virtual Silt_Return Length(const ConstValue& key, size_t& len) const;
        virtual Silt_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;

        virtual Silt_ConstIterator Enumerate() const;
        virtual Silt_Iterator Enumerate();

        virtual Silt_ConstIterator Find(const ConstValue& key) const;
        virtual Silt_Iterator Find(const ConstValue& key);

        struct IteratorElem : public Silt_IteratorElem {
            Silt_IteratorElem* Clone() const;
            void Next();

            off_t next_id;
        };

    protected:
        typedef uint32_t entry_length_t;

        Silt_Return length(const ConstValue& key, size_t& len, bool readahead) const;
        Silt_Return get(const ConstValue& key, Value& data, size_t offset, size_t len, bool readahead) const;

        //int disable_readahead();

        // I/O layer (TODO: make this as a separate class)
        void int_initialize();
        void int_terminate();

        bool int_open(const char* pathname, int flags, mode_t mode);
        bool int_is_open() const;
        bool int_close();
        static bool int_unlink(const char* pathname);

        bool int_pread(char* buf, size_t& count, off_t offset, bool readahead) const;
        bool int_pwritev(const struct iovec* iov, int count, off_t offset);
        bool int_sync(bool blocking);

    private:
        size_t data_len_;

        tbb::atomic<off_t> next_append_id_;
        off_t end_id_;
        tbb::atomic<off_t> next_sync_;

        bool use_buffered_io_only_;

        // I/O layer (TODO: make this as a separate class)
        static const size_t chunk_size_ = 1048576;
        static const size_t page_size_ = 512;
        static const size_t page_size_mask_ = page_size_ - 1;
        static const size_t cache_size_ = 128;

        int fd_buffered_sequential_;
        int fd_buffered_random_;
        int fd_direct_random_;

        mutable tbb::queuing_mutex sync_mutex_;
        mutable tbb::queuing_rw_mutex dirty_chunk_mutex_;
        boost::dynamic_bitset<> dirty_chunk_;
        boost::dynamic_bitset<> syncing_chunk_;

        struct cache_entry {
            bool valid;
            off_t offset;
            char data[page_size_];
        };
        mutable tbb::queuing_rw_mutex cache_mutex_;
        mutable cache_entry cache_[cache_size_];
        mutable tbb::atomic<size_t> cache_hit_;
        mutable tbb::atomic<size_t> cache_miss_;

        class SyncTask : public Task {
        public:
            virtual ~SyncTask();
            virtual void Run();
            FileStore* file_store;
            tbb::queuing_mutex::scoped_lock* sync_lock;
        };
        static TaskScheduler task_scheduler_sync_;

        friend class SyncTask;
    };

} // namespace silt

#endif // #ifndef _FILE_STORE_H_
