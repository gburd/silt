/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#include "silt.h"
#include "configuration.h"
#include <iostream>

namespace silt {

    Silt::Silt()
        : config_(NULL)
    {
    }

    Silt::~Silt()
    {
        delete config_;
        config_ = NULL;
    }

    Silt_Return
    Silt::SetConfig(const Configuration* config)
    {
        if (config_)
            return ERROR;
        config_ = config;
        return OK;
    }

    const Configuration*
    Silt::GetConfig() const
    {
        return config_;
    }

    Silt_Return
    Silt::Create()
    {
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Open()
    {
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::ConvertTo(Silt* new_store) const
    {
        (void)new_store;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Flush()
    {
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Close()
    {
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Destroy()
    {
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Status(const Silt_StatusType& type, Value& status) const
    {
        (void)type; (void)status;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Put(const ConstValue& key, const ConstValue& data)
    {
        (void)key; (void)data;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Append(Value& key, const ConstValue& data)
    {
        (void)key; (void)data;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Delete(const ConstValue& key)
    {
        (void)key;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Contains(const ConstValue& key) const
    {
        (void)key;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Length(const ConstValue& key, size_t& len) const
    {
        (void)key; (void)len;
        return UNSUPPORTED;
    }

    Silt_Return
    Silt::Get(const ConstValue& key, Value& data, size_t offset, size_t len) const
    {
        (void)key; (void)data; (void)offset; (void)len;
        return UNSUPPORTED;
    }

    Silt_ConstIterator
    Silt::Enumerate() const
    {
        std::cerr << "unsupported Enumerate() called" << std::endl;
        return Silt_ConstIterator();
    }

    Silt_Iterator
    Silt::Enumerate()
    {
        std::cerr << "unsupported Enumerate() called" << std::endl;
        return Silt_Iterator();
    }

    Silt_ConstIterator
    Silt::Find(const ConstValue& key) const
    {
        std::cerr << "unsupported Find() called" << std::endl;
        (void)key;
        return Silt_ConstIterator();
    }

    Silt_Iterator
    Silt::Find(const ConstValue& key)
    {
        std::cerr << "unsupported Find() called" << std::endl;
        (void)key;
        return Silt_Iterator();
    }

} // namespace silt

