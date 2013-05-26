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
