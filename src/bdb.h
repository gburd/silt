/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _BDB_H_
#define _BDB_H_

#include "silt.h"
#include "config.h"
#include <tbb/atomic.h>

#ifdef HAVE_LIBDB
#include <db.h>

namespace silt {

    // configuration
    //   <type>: "bdb" (fixed)
    //   <id>: the ID of the store
    //   <file>: the file name prefix of Berkeley Database

    class BDB : public Silt {
    public:
        BDB();
        virtual ~BDB();

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

        virtual Silt_ConstIterator Find(const ConstValue& key) const;
        virtual Silt_Iterator Find(const ConstValue& key);

        struct IteratorElem : public Silt_IteratorElem {
            IteratorElem(const BDB* silt);
            ~IteratorElem();

            Silt_IteratorElem* Clone() const;
            void Next();

            void Increment(bool initial);

            DBC* cursor;
        };

    private:
        DB* dbp_;

        size_t key_len_;
        size_t data_len_;

        tbb::atomic<size_t> size_;

        friend struct IteratorElem;
    };

}  // namespace silt

#endif

#endif  // #ifndef _SILT_SF_H_
