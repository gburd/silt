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
