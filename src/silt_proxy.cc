/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#include "silt_proxy.h"
#include "silt_factory.h"
#include <cassert>

namespace silt {

    Silt_Proxy::Silt_Proxy()
        : store_(NULL)
    {
    }

    Silt_Proxy::~Silt_Proxy()
    {
        if (store_)
            Close();
    }

    Silt_Return
    Silt_Proxy::Create()
    {
        if (store_)
            return ERROR;

        Configuration* store_config = new Configuration(config_, true);
        if (store_config->DeleteNode("child::type") != 0)
            assert(false);
        store_ = Silt_Factory::New(store_config);
        if (!store_) {
            delete store_config;
            return ERROR;
        }

        return store_->Create();
    }

    Silt_Return
    Silt_Proxy::Open()
    {
        if (store_)
            return ERROR;

        Configuration* store_config = new Configuration(config_, true);
        if (store_config->DeleteNode("child::type") != 0)
            assert(false);
        store_ = Silt_Factory::New(store_config);
        if (!store_) {
            delete store_config;
            return ERROR;
        }

        return store_->Open();
    }

    Silt_Return
    Silt_Proxy::ConvertTo(Silt* new_store) const
    {
        if (!store_)
            return ERROR;

        return store_->ConvertTo(new_store);
    }

    Silt_Return
    Silt_Proxy::Flush()
    {
        return store_->Flush();
    }

    Silt_Return
    Silt_Proxy::Close()
    {
        if (!store_)
            return ERROR;

        Silt_Return ret = store_->Close();
        if (ret == OK) {
            delete store_;
            store_ = NULL;
        }
        return ret;
    }

    Silt_Return
    Silt_Proxy::Destroy()
    {
        Configuration* store_config = new Configuration(config_, true);
        if (store_config->DeleteNode("child::type") != 0)
            assert(false);
        Silt* store = Silt_Factory::New(store_config);
        if (!store_) {
            delete store_config;
            return ERROR;
        }

        Silt_Return ret = store->Destroy();

        delete store;
        return ret;
    }

    Silt_Return
    Silt_Proxy::Status(const Silt_StatusType& type, Value& status) const
    {
        return store_->Status(type, status);
    }

    Silt_Return
    Silt_Proxy::Put(const ConstValue& key, const ConstValue& data)
    {
        return store_->Put(key, data);
    }

    Silt_Return
    Silt_Proxy::Append(Value& key, const ConstValue& data)
    {
        return store_->Append(key, data);
    }

    Silt_Return
    Silt_Proxy::Delete(const ConstValue& key)
    {
        return store_->Delete(key);
    }

    Silt_Return
    Silt_Proxy::Contains(const ConstValue& key) const
    {
        return store_->Contains(key);
    }

    Silt_Return
    Silt_Proxy::Length(const ConstValue& key, size_t& len) const
    {
        return store_->Length(key, len);
    }

    Silt_Return
    Silt_Proxy::Get(const ConstValue& key, Value& data, size_t offset, size_t len) const
    {
        return store_->Get(key, data, offset, len);
    }

    Silt_ConstIterator
    Silt_Proxy::Enumerate() const
    {
        return store_->Enumerate();
    }

    Silt_Iterator
    Silt_Proxy::Enumerate()
    {
        return store_->Enumerate();
    }

    Silt_ConstIterator
    Silt_Proxy::Find(const ConstValue& key) const
    {
        return store_->Find(key);
    }

    Silt_Iterator
    Silt_Proxy::Find(const ConstValue& key)
    {
        return store_->Find(key);
    }

} // namespace silt
