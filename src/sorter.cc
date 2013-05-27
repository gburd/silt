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

#include "sorter.h"
#include "debug.h"
#include "configuration.h"
#ifndef HAVE_LIBNSORT
#include <algorithm>
#endif
#include <cassert>

namespace silt {

    Sorter::Sorter()
        : open_(false)
    {
    }

    Sorter::~Sorter()
    {
        if (open_)
            Close();
    }

    Silt_Return
    Sorter::Create()
    {
        if (open_)
            return ERROR;

        if (config_->ExistsNode("child::key-len") == 0)
            key_len_ = atoi(config_->GetStringValue("child::key-len").c_str());
        else
            key_len_ = 0;

        if (config_->ExistsNode("child::data-len") == 0)
            data_len_ = atoi(config_->GetStringValue("child::data-len").c_str());
        else
            data_len_ = 0;

        if (config_->ExistsNode("child::temp-file") == 0)
            temp_file_ = config_->GetStringValue("child::temp-file");
        else
            temp_file_ = "/tmp";

#ifdef HAVE_LIBNSORT
        char buf[1024];

        if (key_len_ != 0 && data_len_ != 0) {
            const char *nsort_definition =
                " -format=size: %zu"
                " -field=Key, char, offset: 0, size: %zu"
                " -key=Key"
                " -temp_file=%s";
            snprintf(buf, sizeof(buf), nsort_definition, key_len_ + data_len_, key_len_, temp_file_.c_str());
        } else {
            // currently not supported
            return ERROR;
        }

        nsort_msg_t ret = nsort_define(buf, 0, NULL, &nsort_ctx_);

        if (ret != NSORT_SUCCESS) {
            fprintf(stderr, "Nsort error: %s\n", nsort_message(&nsort_ctx_));
            return ERROR;
        }

        nsort_buf_.resize(key_len_ + data_len_);
#endif
        open_ = true;
        input_ended_ = false;
        return OK;
    }

#ifndef HAVE_LIBNSORT
    struct _ordering {
        const std::vector<Value> &key_array;

        _ordering(const std::vector<Value> &key_array) : key_array(key_array) {}

        bool operator()(const size_t &i, const size_t &j) {
            // sort key = (entry key, index)
            int cmp = key_array[i].compare(key_array[j]);

            if (cmp != 0)
                return cmp < 0;
            else
                return i < j;   // stable sorting
        };
    };
#endif

    Silt_Return
    Sorter::Flush()
    {
        if (!open_)
            return ERROR;

        if (input_ended_)
            return ERROR;

        input_ended_ = true;
#ifndef HAVE_LIBNSORT
        refs_.reserve(key_array_.size());

        for (size_t i = 0; i < key_array_.size(); i++)
            refs_.push_back(i);

        // uses unstable sorting with stable sort key comparison
        // because std::sort() is often more efficient than std::stable_sort()
        std::sort(refs_.begin(), refs_.end(), _ordering(key_array_));
        refs_it_ = refs_.begin();
#else
        nsort_msg_t ret = nsort_release_end(&nsort_ctx_);

        if (ret != NSORT_SUCCESS)
            fprintf(stderr, "Nsort error: %s\n", nsort_message(&nsort_ctx_));

#endif
        return OK;
    }

    Silt_Return
    Sorter::Close()
    {
        if (!open_)
            return ERROR;

#ifndef HAVE_LIBNSORT
        key_array_.clear();
        data_array_.clear();
        refs_.clear();
#else
        nsort_end(&nsort_ctx_);
        nsort_buf_.resize(0);
#endif
        open_ = false;
        return OK;
    }

    Silt_Return
    Sorter::Put(const ConstValue &key, const ConstValue &data)
    {
        if (!open_)
            return ERROR;

        if (input_ended_)
            return ERROR;

        if (key.size() != key_len_)
            return INVALID_KEY;

        if (data.size() != data_len_)
            return INVALID_DATA;

#ifndef HAVE_LIBNSORT
        NewValue copied_key(key.data(), key.size());
        NewValue copied_value(data.data(), data.size());
        key_array_.push_back(copied_key);
        data_array_.push_back(copied_value);
#else
        memcpy(nsort_buf_.data(), key.data(), key_len_);
        memcpy(nsort_buf_.data() + key_len_, data.data(), data_len_);
        nsort_msg_t ret = nsort_release_recs(nsort_buf_.data(), key_len_ + data_len_, &nsort_ctx_);

        if (ret != NSORT_SUCCESS)
            fprintf(stderr, "Nsort error: %s\n", nsort_message(&nsort_ctx_));

#endif
        return OK;
    }

    Silt_ConstIterator
    Sorter::Enumerate() const
    {
        if (!open_)
            return Silt_ConstIterator();

        if (!input_ended_)
            return Silt_ConstIterator();

        IteratorElem *elem = new IteratorElem(this);
        elem->state = OK;
        elem->Increment(true);
        return Silt_ConstIterator(elem);
    }

    Silt_Iterator
    Sorter::Enumerate()
    {
        if (!open_)
            return Silt_Iterator();

        if (!input_ended_)
            return Silt_Iterator();

        IteratorElem *elem = new IteratorElem(this);
        elem->state = OK;
        elem->Increment(true);
        return Silt_Iterator(elem);
    }

    Sorter::IteratorElem::IteratorElem(const Sorter *silt)
    {
        this->silt = silt;
    }

    Silt_IteratorElem *
    Sorter::IteratorElem::Clone() const
    {
        IteratorElem *elem = new IteratorElem(static_cast<const Sorter *>(silt));
        *elem = *this;
        return elem;
    }

    void
    Sorter::IteratorElem::Next()
    {
        Increment(false);
    }

    void
    Sorter::IteratorElem::Increment(bool initial)
    {
        if (state == END)
            return;

        const Sorter *sorter = static_cast<const Sorter *>(silt);
#ifndef HAVE_LIBNSORT
        std::vector<size_t>::const_iterator &refs_it = sorter->refs_it_;

        if (!initial)
            ++refs_it;

        if (refs_it == sorter->refs_.end()) {
            state = END;
            return;
        }

        state = OK;
        key = sorter->key_array_[*refs_it];
        data = sorter->data_array_[*refs_it];
#else
        (void)initial;
        size_t size = sorter->key_len_ + sorter->data_len_;
        nsort_msg_t ret = nsort_return_recs(sorter->nsort_buf_.data(), &size, &sorter->nsort_ctx_);

        if (ret == NSORT_SUCCESS) {
            assert(size == sorter->key_len_ + sorter->data_len_);
            key = NewValue(sorter->nsort_buf_.data(), sorter->key_len_);
            data = NewValue(sorter->nsort_buf_.data() + sorter->key_len_, sorter->data_len_);
            state = OK;
        } else if (ret == NSORT_END_OF_OUTPUT)
            state = END;
        else {
            fprintf(stderr, "Nsort error: %s\n", nsort_message(&sorter->nsort_ctx_));
            assert(false);
            state = END;
        }

#endif
    }

} // namespace silt
