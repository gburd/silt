/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
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

        virtual Silt_Return Status(const Silt_StatusType& type, Value& status) const;

        virtual Silt_Return Put(const ConstValue& key, const ConstValue& data);
        virtual Silt_Return Append(Value& key, const ConstValue& data);

        virtual Silt_Return Delete(const ConstValue& key);

        virtual Silt_Return Contains(const ConstValue& key) const;
        virtual Silt_Return Length(const ConstValue& key, size_t& len) const;
        virtual Silt_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;

        virtual Silt_ConstIterator Enumerate() const;
        virtual Silt_Iterator Enumerate();

        virtual Silt_ConstIterator Find(const ConstValue& key) const;
        virtual Silt_Iterator Find(const ConstValue& key);

        struct IteratorElem : public Silt_IteratorElem {
            IteratorElem(const Silt_Partition* silt);
            ~IteratorElem();

            Silt_IteratorElem* Clone() const;
            void Next();

            size_t next_store;
            Silt_Iterator store_it;
        };

    private:
        Silt_Return alloc_stores();
        size_t get_partition(const ConstValue& key) const;

        size_t skip_bits_;
        size_t partitions_;
        size_t partition_bits_;

        std::vector<Silt*> stores_;

        friend class IteratorElem;
    };

} // namespace silt

#endif  // #ifndef _SILT_PARTITION_H_
