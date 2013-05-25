/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#include "silt_partition.h"
#include "silt_factory.h"
#include "debug.h"
#include "bit_access.hpp"
#include <sstream>

namespace silt {

    Silt_Partition::Silt_Partition()
    {
    }

    Silt_Partition::~Silt_Partition()
    {
        Close();
    }

    Silt_Return
    Silt_Partition::Create()
    {
        if (stores_.size())
            return ERROR;

        alloc_stores();

        for (size_t i = 0; i < partitions_; i++) {
            if (stores_[i]->Create() != OK) {
                return ERROR;
            }
        }

        return OK;
    }

    Silt_Return
    Silt_Partition::Open()
    {
        if (stores_.size())
            return ERROR;

        alloc_stores();

        for (size_t i = 0; i < partitions_; i++) {
            if (stores_[i]->Open() != OK) {
                return ERROR;
            }
        }

        return OK;
    }

    Silt_Return
    Silt_Partition::Flush()
    {
        if (!stores_.size())
            return ERROR;

        for (std::vector<Silt*>::iterator it = stores_.begin(); it != stores_.end(); ++it) {
            Silt_Return ret = (*it)->Flush();
            if (ret != OK)
                return ret;
        }
        return OK;
    }

    Silt_Return
    Silt_Partition::Close()
    {
        if (!stores_.size())
            return ERROR;

        for (std::vector<Silt*>::iterator it = stores_.begin(); it != stores_.end(); ++it) {
            if (*it) {
                Silt_Return ret = (*it)->Close();
                if (ret != OK)
                    return ret;
                delete *it;
                *it = NULL;
            }
        }
        stores_.clear();
        return OK;
    }

    Silt_Return
    Silt_Partition::Destroy()
    {
        for (size_t i = 0; i < partitions_; i++) {
            Configuration* storeConfig = new Configuration(config_, true);
            storeConfig->SetContextNode("child::store");

            char buf[1024];
            snprintf(buf, sizeof(buf), "%s_%zu", config_->GetStringValue("child::id").c_str(), i);
            storeConfig->SetStringValue("child::id", buf);

            Silt* store = Silt_Factory::New(storeConfig);
            if (!store) {
                delete storeConfig;
                return ERROR;
            }
            store->Destroy();
            delete store;
        }

        return OK;
    }

	Silt_Return
	Silt_Partition::Status(const Silt_StatusType& type, Value& status) const
	{
        std::ostringstream oss;
        switch (type) {
        case NUM_DATA:
        case NUM_ACTIVE_DATA:
		case CAPACITY:
        case MEMORY_USE:
        case DISK_USE:
            {
                bool first = true;
                Value status_part;
                oss << '[';
                for (std::vector<Silt*>::const_iterator it = stores_.begin(); it != stores_.end(); ++it) {
                    if (first)
                        first = false;
                    else
                        oss << ',';
                    Silt_Return ret = (*it)->Status(type, status_part);
                    if (ret != OK)
                        return UNSUPPORTED;
                    oss << status_part.str();
                }
                oss << ']';
            }
            break;
        default:
            return UNSUPPORTED;
        }
        status = NewValue(oss.str());
        return OK;
    }

    Silt_Return
    Silt_Partition::Put(const ConstValue& key, const ConstValue& data)
    {
        if (!stores_.size())
            return ERROR;

        size_t p = get_partition(key);
        return stores_[p]->Put(key, data);
    }

    Silt_Return
    Silt_Partition::Append(Value& key, const ConstValue& data)
    {
        if (!stores_.size())
            return ERROR;

        size_t p = get_partition(key);
        return stores_[p]->Append(key, data);
    }

    Silt_Return
    Silt_Partition::Delete(const ConstValue& key)
    {
        if (!stores_.size())
            return ERROR;

        size_t p = get_partition(key);
        return stores_[p]->Delete(key);
    }

    Silt_Return
    Silt_Partition::Contains(const ConstValue& key) const
    {
        if (!stores_.size())
            return ERROR;

        size_t p = get_partition(key);
        return stores_[p]->Contains(key);
    }

    Silt_Return
    Silt_Partition::Length(const ConstValue& key, size_t& len) const
    {
        if (!stores_.size())
            return ERROR;

        size_t p = get_partition(key);
        return stores_[p]->Length(key, len);
    }

    Silt_Return
    Silt_Partition::Get(const ConstValue& key, Value& data, size_t offset, size_t len) const
    {
        if (!stores_.size())
            return ERROR;

        size_t p = get_partition(key);
        return stores_[p]->Get(key, data, offset, len);
    }

    Silt_ConstIterator
    Silt_Partition::Enumerate() const
    {
        IteratorElem* elem = new IteratorElem(this);
        elem->next_store = 0;
        elem->Next();
        return Silt_ConstIterator(elem);
    }

    Silt_Iterator
    Silt_Partition::Enumerate()
    {
        IteratorElem* elem = new IteratorElem(this);
        elem->next_store = 0;
        elem->Next();
        return Silt_Iterator(elem);
    }

    Silt_ConstIterator
    Silt_Partition::Find(const ConstValue& key) const
    {
        if (!stores_.size())
            return Silt_ConstIterator();

        size_t p = get_partition(key);
        return stores_[p]->Find(key);
    }

    Silt_Iterator
    Silt_Partition::Find(const ConstValue& key)
    {
        if (!stores_.size())
            return Silt_Iterator();

        size_t p = get_partition(key);
        return stores_[p]->Find(key);
    }

    Silt_Return
    Silt_Partition::alloc_stores()
    {
        skip_bits_ = atoi(config_->GetStringValue("child::skip_bits").c_str());

        partitions_ = atoi(config_->GetStringValue("child::partitions").c_str());
        partition_bits_ = 0;
        while ((1u << partition_bits_) < partitions_)
            partition_bits_++;

        if (partitions_ == 0) {
            DPRINTF(2, "Silt_Partition::alloc_stores(): non-zero partitions required\n");
            return ERROR;
        }

        if ((1u << partition_bits_) != partitions_) {
            DPRINTF(2, "Silt_Partition::alloc_stores(): # of partitions should be power of 2\n");
            return ERROR;
        }

        for (size_t i = 0; i < partitions_; i++) {
            Configuration* storeConfig = new Configuration(config_, true);
            storeConfig->SetContextNode("child::store");

            char buf[1024];
            snprintf(buf, sizeof(buf), "%s_%zu", config_->GetStringValue("child::id").c_str(), i);
            storeConfig->SetStringValue("child::id", buf);

            Silt* store = Silt_Factory::New(storeConfig);
            if (!store) {
                delete storeConfig;
                return ERROR;
            }
            stores_.push_back(store);
        }
        return OK;
    }

    size_t
    Silt_Partition::get_partition(const ConstValue& key) const
    {
        std::size_t bits = 0;
        std::size_t index = 0;
        while (bits < partition_bits_)
		{
            index <<= 1;
            if ((skip_bits_ + bits) / 8 > key.size())
			{
				// too short key
				assert(false);
                break;
            }
            if (cindex::bit_access::get(reinterpret_cast<const uint8_t*>(key.data()), skip_bits_ + bits))
                index |= 1;
            bits++;
        }

        return index;
    }

    Silt_Partition::IteratorElem::IteratorElem(const Silt_Partition* silt)
    {
        this->silt = silt;
    }

    Silt_Partition::IteratorElem::~IteratorElem()
    {
    }

    Silt_IteratorElem*
    Silt_Partition::IteratorElem::Clone() const
    {
        IteratorElem* elem = new IteratorElem(static_cast<const Silt_Partition*>(silt));
        *elem = *this;
        return elem;
    }

    void
    Silt_Partition::IteratorElem::Next()
    {
        const Silt_Partition* silt_part = static_cast<const Silt_Partition*>(silt);

        if (!store_it.IsEnd())
            ++store_it;

        while (store_it.IsEnd()) {
            if (next_store >= silt_part->stores_.size()) {
                state = END;
                return;
            }
            store_it = silt_part->stores_[next_store++]->Enumerate();
        }

        state = store_it->state;
        key = store_it->key;
        data = store_it->data;
    }

} // namespace silt

