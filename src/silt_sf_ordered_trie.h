/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _SILT_SF_ORDERED_TRIE_H_
#define _SILT_SF_ORDERED_TRIE_H_

#include "silt_factory.h"
#include "bucketing_index.hpp"
#include "twolevel_absoff_bucketing.hpp"

namespace silt
{
    // configuration
    //   <type>: "sf_ordered_trie" (fixed)
    //   <id>: the ID of the store
    //   <key-len>: the fixed length of all keys
    //   <data-len>: the fixed length of all data
    //   <keys-per-block>: the number of key-value pairs in a I/O block
    //   <size>: the total number of entries in the store
    //   <bucket-size>: the number of entires in each bucket
    //   <skip-bits>: the number of MSBs to ignore when calculating the bucket number
    //   <datastore>: the configuration for the data store
    //                <id>: will be assigned by Silt_SF_Ordered_Trie
    //                <data-len>: will be set by Silt_SF_Ordered_Trie

    class Silt_SF_Ordered_Trie : public Silt
    {
    public:
        Silt_SF_Ordered_Trie();
        virtual ~Silt_SF_Ordered_Trie();

        virtual Silt_Return Create();
        virtual Silt_Return Open();

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

        //virtual Silt_ConstIterator Find(const ConstValue& key) const;
        //virtual Silt_Iterator Find(const ConstValue& key);

        struct IteratorElem : public Silt_IteratorElem {
            IteratorElem(const Silt_SF_Ordered_Trie* silt);

            Silt_IteratorElem* Clone() const;
            void Next();
            void Parse();

            Silt_Iterator data_store_it;
        };

	private:
        typedef cindex::bucketing_index<cindex::twolevel_absoff_bucketing<> > index_type;
        index_type* index_;
        Silt* data_store_;

        size_t key_len_;
        size_t data_len_;
        size_t keys_per_block_;

        size_t size_;
        size_t bucket_size_;

        size_t skip_bits_;

        size_t actual_size_;
    };
}  // namespace silt

#endif  // #ifndef _SILT_SF_ORDERED_TRIE_H_
