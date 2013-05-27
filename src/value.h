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

#ifndef _VALUE_H_
#define _VALUE_H_

#include "basic_types.h"
#include <tr1/type_traits>
#include <cstdlib>
#include <string>

namespace silt {

    class ConstValue {
        public:
            explicit ConstValue()
                : data_(NULL), size_(0), capacity_(0), share_count_ptr_(NULL) {
                reset();
            }

            ConstValue(const ConstValue &rhs) {
                copy_from(rhs);
            }

            ~ConstValue() {
                // Decendant classes must not define a destructor
                reset();
            }

            void reset() {
                if (share_count_ptr_ && !--*share_count_ptr_) {
                    delete [] data_;
                    delete share_count_ptr_;
                }

                data_ = NULL;
                size_ = 0;
                capacity_ = 0;
                share_count_ptr_ = NULL;
            }

            ConstValue &operator=(const ConstValue &rhs) {
                reset();
                copy_from(rhs);
                return *this;
            }

            bool operator==(const ConstValue &rhs) const {
                return size_ == rhs.size_ && memcmp(data_, rhs.data_, size_) == 0;
            }

            bool operator!=(const ConstValue &rhs) const {
                return !(*this == rhs);
            }

            bool operator<(const ConstValue &rhs) const {
                return compare(rhs) < 0;
            }

            bool operator<=(const ConstValue &rhs) const {
                return compare(rhs) <= 0;
            }

            bool operator>(const ConstValue &rhs) const {
                return compare(rhs) > 0;
            }

            bool operator>=(const ConstValue &rhs) const {
                return compare(rhs) > 0;
            }

            int compare(const ConstValue &rhs) const {
                size_t llen = size();
                size_t rlen = rhs.size();
                size_t mlen = llen <= rlen ? llen : rlen;
                int cmp = memcmp(data(), rhs.data(), mlen);

                if (cmp != 0)
                    return cmp;
                else
                    return llen - rlen;
            }

            const char *data() const {
                return data_;
            }

            const size_t &size() const {
                return size_;
            }

            std::string str() const {
                return std::string(data_, size_);
            }

            const size_t &capacity() const {
                return capacity_;
            }

            template <typename T>
            const T &as() const {
                return *reinterpret_cast<const T *>(data_);
            }

            template<typename T>
            bool as_number(T *v) const {
                if (std::tr1::is_unsigned<T>::value) {
                    switch (size_) {
                        case 1:
                            *v = static_cast<T>(*reinterpret_cast<const uint8_t *>(data_));
                            return true;

                        case 2:
                            *v = static_cast<T>(*reinterpret_cast<const uint16_t *>(data_));
                            return true;

                        case 4:
                            *v = static_cast<T>(*reinterpret_cast<const uint32_t *>(data_));
                            return true;

                        case 8:
                            *v = static_cast<T>(*reinterpret_cast<const uint64_t *>(data_));
                            return true;
                    }
                } else {
                    switch (size_) {
                        case 1:
                            *v = static_cast<T>(*reinterpret_cast<const int8_t *>(data_));
                            return true;

                        case 2:
                            *v = static_cast<T>(*reinterpret_cast<const int16_t *>(data_));
                            return true;

                        case 4:
                            *v = static_cast<T>(*reinterpret_cast<const int32_t *>(data_));
                            return true;

                        case 8:
                            *v = static_cast<T>(*reinterpret_cast<const int64_t *>(data_));
                            return true;
                    }
                }

                return false;
            }

            template<typename T>
            T as_number(const T &default_v = 0) const {
                T v;

                if (as_number<T>(&v))
                    return v;
                else
                    return default_v;
            }

        protected:
            void init_copy(const char *data, size_t size, size_t capacity) {
                // must be called at most once
                if (capacity < size)
                    capacity = size;

                data_ = new char[capacity];
                memcpy(const_cast<char *>(data_), data, size);
                size_ = size;
                capacity_ = capacity;
                share_count_ptr_ = new size_t(1);
            }

            void init_ref(const char *data, size_t size, size_t capacity) {
                // must be called at most once
                if (capacity < size)
                    capacity = size;

                data_ = data;
                size_ = size;
                capacity_ = capacity;
                share_count_ptr_ = NULL;
            }

            void copy_from(const ConstValue &rhs) {
                // reset() is required if this object was holding data
                data_ = rhs.data_;
                size_ = rhs.size_;
                capacity_ = rhs.capacity_;

                if ((share_count_ptr_ = rhs.share_count_ptr_))
                    ++*rhs.share_count_ptr_;
            }

            const char *data_;
            size_t size_;
            size_t capacity_;
            size_t *share_count_ptr_;
    };

    class Value : public ConstValue {
        public:
            explicit Value() : ConstValue() { }

            Value(const Value &rhs) : ConstValue(rhs) { }

            explicit Value(const ConstValue &rhs) : ConstValue(rhs) { } // not safe initialization

            Value &operator=(const Value &rhs) {
                return (*this = static_cast<const ConstValue &>(rhs));
            }

            Value &operator=(const ConstValue &rhs) {
                reset();
                copy_from(rhs);
                return *this;
            }

            const char *data() const {
                return data_;
            }
            char *data() {
                return const_cast<char *>(data_);
            }

            void resize(size_t new_size, bool preserve = true) {
                if (data_ != NULL && (size_ >= new_size || capacity_ >= new_size)) {
                    size_ = new_size;
                    return;
                }

                if (preserve) {
                    char *new_data = new char[new_size];
                    memcpy(new_data, data_, size_);
                    reset();
                    data_ = new_data;
                    size_ = new_size;
                    capacity_ = new_size;
                    share_count_ptr_ = new size_t(1);
                } else {
                    reset();
                    data_ = new char[new_size];
                    size_ = new_size;
                    capacity_ = new_size;
                    share_count_ptr_ = new size_t(1);
                }
            }

        protected:
            void init_ref(char *data, size_t size, size_t capacity) {
                ConstValue::init_ref(const_cast<const char *>(data), size, capacity);
            }
    };

    class NewValue : public Value {
        public:
            explicit NewValue(const char *data, size_t size, size_t capacity = 0) {
                init_copy(data, size, capacity);
            }

            template <typename T>
            explicit NewValue(const T *value) {
                init_copy(reinterpret_cast<const char *>(value), sizeof(T), sizeof(T));
            }

            explicit NewValue(const std::string &s) {
                init_copy(s.data(), s.size(), s.capacity());
            }
    };

    class RefValue : public Value {
        public:
            explicit RefValue(char *data, size_t size, size_t capacity = 0) {
                init_ref(data, size, capacity);
            }

            template <typename T>
            explicit RefValue(T *value) {
                init_ref(reinterpret_cast<char *>(value), sizeof(T), sizeof(T));
            }
    };

    class ConstRefValue : public ConstValue {
        public:
            explicit ConstRefValue(const char *data, size_t size, size_t capacity = 0) {
                init_ref(data, size, capacity);
            }

            template <typename T>
            explicit ConstRefValue(const T *value) {
                init_ref(reinterpret_cast<const char *>(value), sizeof(T), sizeof(T));
            }

            explicit ConstRefValue(const std::string &s) {
                init_ref(s.data(), s.size(), s.capacity());
            }
    };

    template <size_t Size>
    class SizedValue : public RefValue {
        public:
            explicit SizedValue() : RefValue(buf_, 0, buf_size_) {}

        protected:
            static const size_t buf_size_ = Size;
            char buf_[buf_size_];
    };

} // namespace silt

#endif  // #ifndef _VALUE_H_
