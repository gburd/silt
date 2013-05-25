/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _SILT_PROXY_H_
#define _SILT_PROXY_H_

#include "silt.h"

namespace silt {

    // configuration
    //   <type>: "proxy" (fixed)
    //   <(others)>: used by the main store

    class Silt_Proxy : public Silt {
    public:
        Silt_Proxy();
        virtual ~Silt_Proxy();

        virtual Silt_Return Create();
        virtual Silt_Return Open();

        virtual Silt_Return ConvertTo(Silt* new_store) const;

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

    protected:
        Silt* store_;
    };

} // namespace silt

#endif  // #ifndef _SILT_PROXY_H_
