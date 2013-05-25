/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _SILT_SF_H_
#define _SILT_SF_H_

#include "silt.h"

namespace silt {

    // configuration
    //   <type>: "sf" (fixed)
    //   <id>: the ID of the store
    //   <file>: the file name prefix to store bookkeeping information for persistency
    //   <key-len>: the length of keys -- zero for variable-length keys (default), a positive integer for fixed-length keys (space optimization can be applied in the future)
    //   <data-len>: the length of data -- zero for variable-length data (default), a positive integer for fixed-length data (space optimization can be applied in the future)
    //   <no-delete>: omit support for deletion (not implemented yet)
    //   <keep-datastore-on-conversion>: when converting another Silt_SF store to this store, assume that the previous hash table preserves the same data layout in this data store; datastore->ConvertTo() is used instead of data reorganization based on side-by-side hash table comparison.
    //   <hashtable>: the configuration for the hash table
    //                <id>: will be assigned by Silt_SF
    //   <datastore>: the configuration for the insert-order-preserving data store
    //                <id>: will be assigned by Silt_SF
    //                <data-len>: will be set by Silt_SF (zero if either key length or data length is zero, (header size) + key length + data length otherwise)

    class Silt_SF : public Silt {
    public:
        Silt_SF();
        virtual ~Silt_SF();

        virtual Silt_Return Create();
        virtual Silt_Return Open();

        virtual Silt_Return ConvertTo(Silt* new_store) const;

        virtual Silt_Return Flush();
        virtual Silt_Return Close();

        virtual Silt_Return Destroy();

        virtual Silt_Return Status(const Silt_StatusType& type, Value& status) const;

        virtual Silt_Return Put(const ConstValue& key, const ConstValue& data);
        //virtual Silt_Return Append(Value& key, const ConstValue& data);

        virtual Silt_Return Delete(const ConstValue& key);

        virtual Silt_Return Contains(const ConstValue& key) const;
        virtual Silt_Return Length(const ConstValue& key, size_t& len) const;
        virtual Silt_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;

        virtual Silt_ConstIterator Enumerate() const;
        virtual Silt_Iterator Enumerate();

        //virtual Silt_ConstIterator Find(const ConstValue& key) const;
        //virtual Silt_Iterator Find(const ConstValue& key);

        struct IteratorElem : public Silt_IteratorElem {
            IteratorElem(const Silt_SF* silt);
            ~IteratorElem();

            Silt_IteratorElem* Clone() const;
            void Next();

            void Increment(bool initial);

            Silt_Iterator data_store_it;
        };

    protected:
        Silt_Return InsertEntry(const ConstValue& key, const ConstValue& data, bool isDelete);

        struct DbHeader {
            uint64_t magic_number;
            uint64_t num_elements;
            uint64_t num_active_elements;
            //double max_deleted_ratio;
            //double max_load_factor;
            //keyType keyFormat;
        } __attribute__((__packed__));

        struct DataHeaderFull {
            uint16_t type;  // 0 = unused, 1 = insert/update, 2 = delete
            uint16_t key_len;
        } __attribute__((__packed__));

        struct DataHeaderSimple {
            uint16_t type;  // 0 = unused, 1 = insert/update, 2 = delete
            uint16_t padding;
        } __attribute__((__packed__));

        /*
        static const int DSReadMin = 2048;
        struct DataHeaderExtended {
            //uint32_t data_length;
            uint32_t key_length;
            bool deleteLog;
            char partial_data[DSReadMin-sizeof(struct DataHeader)];
        } __attribute__((__packed__));
        */

    private:
        Silt_Return WriteHeaderToFile() const __attribute__ ((warn_unused_result));
        Silt_Return ReadHeaderFromFile() __attribute__ ((warn_unused_result));

        DbHeader* header_;
        Silt* hash_table_;

        Silt* data_store_;

        size_t key_len_;
        size_t data_len_;

        mutable pthread_rwlock_t silt_lock_;

        friend struct IteratorElem;
    };

}  // namespace silt

#endif  // #ifndef _SILT_SF_H_
