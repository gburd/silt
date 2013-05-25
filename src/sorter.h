/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _SORTER_H_
#define _SORTER_H_

#include "silt.h"

#include "config.h"
#ifndef HAVE_LIBNSORT
#include <vector>
#else
#include <nsort.h>
#endif

namespace silt {

    // configuration
    //   <type>: "sorter" (fixed)
    //   <key-len>: the length of keys -- zero for variable-length keys (default), a positive integer for fixed-length keys
    //   <data-len>: the length of data -- zero for variable-length data (default), a positive integer for fixed-length data
    //   <temp-file>: the path of the temporary files/directory. "/tmp" is the default.

    class Sorter : public Silt {
    public:
        Sorter();
        virtual ~Sorter();

        virtual Silt_Return Create();
        //virtual Silt_Return Open();

        virtual Silt_Return Flush();
        virtual Silt_Return Close();

        //virtual Silt_Return Destroy();

        virtual Silt_Return Put(const ConstValue& key, const ConstValue& data);
        //virtual Silt_Return Append(Value& key, const ConstValue& data);

        //virtual Silt_Return Delete(const ConstValue& key);

        //virtual Silt_Return Contains(const ConstValue& key) const;
        //virtual Silt_Return Length(const ConstValue& key, size_t& len) const;
        //virtual Silt_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;

        virtual Silt_ConstIterator Enumerate() const;
        virtual Silt_Iterator Enumerate();

        //virtual Silt_ConstIterator Find(const ConstValue& key) const;
        //virtual Silt_Iterator Find(const ConstValue& key);
    
        struct IteratorElem : public Silt_IteratorElem {
            IteratorElem(const Sorter* silt);

            Silt_IteratorElem* Clone() const;
            void Next();
            
            void Increment(bool initial);
        };

    protected:
        bool open_;
        bool input_ended_;

        size_t key_len_;
        size_t data_len_;

        std::string temp_file_;

#ifndef HAVE_LIBNSORT
        std::vector<Value> key_array_;
        std::vector<Value> data_array_;
        std::vector<size_t> refs_;
        mutable std::vector<size_t>::const_iterator refs_it_;
#else
        mutable nsort_t nsort_ctx_;
        mutable Value nsort_buf_;
#endif
    };

} // namespace silt

#endif  // #ifndef _SORTER_H_
