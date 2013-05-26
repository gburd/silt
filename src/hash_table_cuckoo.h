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

#ifndef _HASH_TABLE_CUCKOO_H_
#define _HASH_TABLE_CUCKOO_H_

#include "basic_types.h"
#include "silt.h"

#include <cassert>
#include "print.h"
#include "debug.h"

#include <string>


//#define SPLIT_HASHTABLE

using namespace std;
namespace silt {

class Configuration;

// configuration
//   <type>: "cuckoo" (fixed)
//   <id>: the ID of the file store
//   <file>: the file name prefix to store the hash table for startup
//   <hash-table-size>: the number of entries that the hash table is expected to hold
//   <use-offset>: 1 (default): use an explicit offset field of 4 bytes
//                 0: do not use offsets; a location in the hash table becomes an offset

class HashTableCuckoo : public Silt {
    /*
     * parameters for cuckoo
     */
    static const uint32_t NUMHASH = 2;
    static const uint32_t ASSOCIATIVITY = 4;

    static const uint32_t NUMVICTIM = 2; // size of victim table
    static const uint32_t MAX_CUCKOO_COUNT = 128;
    /*
     * make sure KEYFRAGBITS + VALIDBITS <= 16
     */
    static const uint32_t KEYFRAGBITS = 15;
    static const uint32_t KEYFRAGMASK = (1 << KEYFRAGBITS) - 1;
    static const uint32_t VALIDBITMASK = KEYFRAGMASK + 1;
    static const uint32_t KEYPRESENTMASK =  VALIDBITMASK | KEYFRAGMASK;

protected:
    struct TagValStoreEntry {
        char     tag_vector[ASSOCIATIVITY * ( KEYFRAGBITS + 1) / 8];
        uint32_t val_vector[ASSOCIATIVITY];
    } __attribute__((__packed__));

    struct TagStoreEntry {
        char     tag_vector[ASSOCIATIVITY * ( KEYFRAGBITS + 1) / 8];
    } __attribute__((__packed__));

public:
    HashTableCuckoo();
    virtual ~HashTableCuckoo();

    virtual Silt_Return Create();
    virtual Silt_Return Open();

    virtual Silt_Return Flush();
    virtual Silt_Return Close();

    virtual Silt_Return Destroy();

    virtual Silt_Return Status(const Silt_StatusType& type, Value& status) const;

    virtual Silt_Return Put(const ConstValue& key, const ConstValue& data);

    virtual Silt_Return ConvertTo(Silt* new_store) const;


    // not supported by now.
    // virtual Silt_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;
    //virtual Silt_Return Append(Value& key, const ConstValue& data);
    //virtual Silt_Return Delete(const ConstValue& key);
    //virtual Silt_Return Contains(const ConstValue& key) const;
    //virtual Silt_Return Length(const ConstValue& key, size_t& len) const;

    virtual Silt_ConstIterator Enumerate() const;
    virtual Silt_Iterator Enumerate();

    virtual Silt_ConstIterator Find(const ConstValue& key) const;
    virtual Silt_Iterator Find(const ConstValue& key);

    struct IteratorElem : public Silt_IteratorElem {
        Silt_IteratorElem* Clone() const;
        void Next();
        Silt_Return Replace(const ConstValue& data);
        uint32_t keyfrag[NUMHASH];
        uint32_t current_keyfrag_id;
        uint32_t current_index;
        uint32_t current_way;
    };

protected:
    int WriteToFile();
    int ReadFromFile();

private:
    TagValStoreEntry* hash_table_;
    TagStoreEntry*    fpf_table_;

    uint32_t  max_index_;
    uint32_t  max_entries_;
    uint32_t  current_entries_;

    inline bool valid(uint32_t index, uint32_t way) const {
        uint32_t pos = way * (KEYFRAGBITS + 1);
        uint32_t offset = pos >> 3;
        uint32_t tmp;
        // this is not big-endian safe, and it accesses beyond the end of the table
        /*
        if (hash_table_)
           tmp = *((uint32_t *) (hash_table_[index].tag_vector + offset));
        else
           tmp = *((uint32_t *) (fpf_table_[index].tag_vector + offset));
        tmp = tmp >> (pos & 7);
        */
        // specialized code
        assert(KEYFRAGBITS == 15);
        if (hash_table_)
            tmp = *((uint16_t *) (hash_table_[index].tag_vector + offset));
        else
            tmp = *((uint16_t *) (fpf_table_[index].tag_vector + offset));
        return (tmp & VALIDBITMASK);
    }

    // read from tagvector in the hashtable
    inline uint32_t tag(uint32_t index, uint32_t way) const {
        uint32_t pos = way * (KEYFRAGBITS + 1);
        uint32_t offset = pos >> 3;
        uint32_t tmp;
        // this is not big-endian safe, and it accesses beyond the end of the table
        /*
        if (hash_table_)
            tmp = *((uint32_t *) (hash_table_[index].tag_vector + offset));
        else
            tmp = *((uint32_t *) (fpf_table_[index].tag_vector + offset));
        tmp = tmp >> (pos & 7);
        */
        // specialized code
        assert(KEYFRAGBITS == 15);
        if (hash_table_)
            tmp = *((uint16_t *) (hash_table_[index].tag_vector + offset));
        else
            tmp = *((uint16_t *) (fpf_table_[index].tag_vector + offset));
        return (tmp & KEYFRAGMASK);
    }

    inline uint32_t val(uint32_t index, uint32_t way) const {
        if (hash_table_)
            return hash_table_[index].val_vector[way];
        else
            return index * ASSOCIATIVITY + way;
    }

    // store keyfrag + validbit to  tagvector in the hashtable
    void store(uint32_t index, uint32_t way, uint32_t keypresent, uint32_t id) {
        assert(hash_table_);
        assert(way < ASSOCIATIVITY);
        uint32_t pos = way * (KEYFRAGBITS + 1);
        uint32_t offset = pos >> 3;
        uint32_t shift = pos & 7;
        // this is not big-endian safe, and it accesses beyond the end of the table
        /*
        uint32_t *p= (uint32_t *) (hash_table_[index].tag_vector + offset);
        uint32_t tmp = *p;
        */
        // specialized code
        assert(KEYFRAGBITS == 15);
        uint16_t *p= (uint16_t *) (hash_table_[index].tag_vector + offset);
        uint16_t tmp = *p;
        tmp &= ~( KEYPRESENTMASK << shift);
        *p = tmp | ( keypresent << shift);;
        hash_table_[index].val_vector[way] = id;
    }



    /* read the keyfragment from the key */
    inline uint32_t keyfrag(const ConstValue& key, uint32_t keyfrag_id) const {
        assert(key.size() >= sizeof(uint32_t));
        // take the last 4 bytes
        uint32_t tmp = *((uint32_t *) (key.data() + key.size() - 4));
        tmp = (tmp >> (keyfrag_id * KEYFRAGBITS)) & KEYFRAGMASK;

#ifdef DEBUG
        // DPRINTF(2, "\t\t    key=\t");
        // print_payload((const u_char*) key.data(), key.size(), 4);
        // DPRINTF(2, "\t\t%dth tag=\t", keyfrag_index);
        // print_payload((const u_char*) &tmp, 2, 4);
#endif
        return tmp;
    }


    /* check if the given row has a free slot, return its way */
    uint32_t freeslot(uint32_t index)  {
        uint32_t way;
        for (way = 0; way < ASSOCIATIVITY; way++) {
            DPRINTF(4, "check ... (%d, %d)\t", index, way);
            if (!valid(index, way)) {
                DPRINTF(4, "not used!\n");
                return way;
            }
            DPRINTF(4, "used...\n");
        }
        return way;
    }

    friend class IteratorElem;
};

} // namespace silt

#endif // #ifndef _HASH_TABLE_CUCKOO_H_
