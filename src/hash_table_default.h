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

#ifndef _HASH_TABLE_DEFAULT_H_
#define _HASH_TABLE_DEFAULT_H_

#include "basic_types.h"
#include "silt.h"
#include <tbb/spin_mutex.h>

namespace silt {

class Configuration;

// configuration
//   <type>: "default" (fixed)
//   <id>: the ID of the file store
//   <file>: the file name prefix to store the hash table for startup
//   <hash-table-size>: the number of entries that the hash table is expected to hold
//   <probing-mode>: "linear" (default) for linear probing
//                   "quadratic" for quadratic probing

class HashTableDefault : public Silt {
    // Incrementing keyfragbits above 15 requires
    // more modifications to code (e.g. hashkey is 16 bits in (Insert())
    static const uint32_t KEYFRAGBITS = 15;
    static const uint32_t KEYFRAGMASK = (1 << KEYFRAGBITS) - 1;
    static const uint32_t INDEXBITS = 16;
    static const uint32_t INDEXMASK = (1 << INDEXBITS) - 1;
    static const uint32_t VALIDBITMASK = KEYFRAGMASK+1;

    static const double EXCESS_BUCKET_FACTOR;
    static const double MAX_DELETED_RATIO;
    static const double MAX_LOAD_FACTOR;
    static const double PROBES_BEFORE_REHASH;

protected:
    /*
      Hash Entry Format
      D = Is slot deleted: 1 means deleted, 0 means not deleted.  Needed for lazy deletion
      V = Is slot empty: 0 means empty, 1 means taken
      K = Key fragment
      O = Offset bits
      ________________________________________________
      |DVKKKKKKKKKKKKKKOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO|
      ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
    */
    struct HashEntry {
        uint16_t present_key;
        uint32_t id;
    } __attribute__((__packed__));


public:
    HashTableDefault();
    virtual ~HashTableDefault();

    virtual Silt_Return Create();
    virtual Silt_Return Open();

    virtual Silt_Return ConvertTo(Silt* new_store) const;

    virtual Silt_Return Flush();
    virtual Silt_Return Close();

    virtual Silt_Return Destroy();

    virtual Silt_Return Status(const Silt_StatusType& type, Value& status) const;

    virtual Silt_Return Put(const ConstValue& key, const ConstValue& data);
    //virtual Silt_Return Append(Value& key, const ConstValue& data);

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
        Silt_Return Replace(const ConstValue& data);

        uint16_t vkey;

        size_t current_probe_count;
        size_t max_probe_count;

        size_t next_hash_fn_index;

        size_t first_index;
        size_t current_index;

        bool skip_unused;
    };

protected:
    int WriteToFile();
    int ReadFromFile();

private:
    HashEntry* hash_table_;
    size_t hash_table_size_;
    size_t current_entries_;

    // probe increment function coefficients
    double c_1_;
    double c_2_;

    mutable tbb::spin_mutex cache_mutex_;
    mutable Value cached_lookup_key_;
    mutable size_t cached_unused_index_;

    inline bool valid(int32_t index) const {
        return (hash_table_[index].present_key & VALIDBITMASK);
    }

    inline uint16_t verifykey(int32_t index) const {
        return (hash_table_[index].present_key & KEYFRAGMASK);
    }

    inline uint16_t keyfragment_from_key(const ConstValue& key) const {
        if (key.size() < sizeof(uint16_t)) {
            return (uint16_t) (key.data()[0] & KEYFRAGMASK);
        }
        return (uint16_t) (((key.data()[key.size()-2]<<8) + (key.data()[key.size()-1])) & KEYFRAGMASK);
    }

    friend struct IteratorElem;
};

} // namespace silt

#endif // #ifndef _HASH_TABLE_DEFAULT_H_
