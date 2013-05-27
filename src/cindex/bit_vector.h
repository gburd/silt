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

#pragma once

#include "common.h"
#include "block_info.h"
#include "bit_access.h"

namespace cindex
{
	template<typename BlockType = uint32_t>
	class bit_vector
	{
	public:
		typedef BlockType block_type;

		bit_vector();

		~bit_vector();

		void clear();

		std::size_t
		size() const CINDEX_WARN_UNUSED_RESULT
		{
			return size_;
		}

		// single bit operations
		void
		push_back(bool b)
		{
			if (size_ == capacity_)
			{
				if (capacity_ == 0)
					capacity_ = 1;
				else
					capacity_ <<= 1;
				resize();
			}

			if (b)
				bit_access::set(buf_, size_);
			size_++;
		}

		void
		pop_back()
		{
			assert(size_ > 0);
			size_--;
			bit_access::unset(buf_, size_);
		}

		bool
		operator[](std::size_t i) const CINDEX_WARN_UNUSED_RESULT
		{
			assert(i < size_);
			return bit_access::get(buf_, i);
		}

		bool
		operator[](std::size_t i) CINDEX_WARN_UNUSED_RESULT
		{
			assert(i < size_);
			return bit_access::get(buf_, i);
		}

		// multiple bit operations
		template<typename SrcBlockType>
		void
		append(const bit_vector<SrcBlockType>& r, std::size_t i, std::size_t len)
		{
			assert(i + len <= r.size());
			std::size_t target_size = size_ + r.size();
			if (target_size > capacity_)
			{
				if (capacity_ == 0)
					capacity_ = 1;
				while (target_size > capacity_)
					capacity_ <<= 1;
				resize();
			}

			bit_access::copy_set(buf_, size_, r.buf_, i, len);
			size_ = target_size;
		}

		template<typename SrcBlockType>
		void
		append(const SrcBlockType* r, std::size_t i, std::size_t len)
		{
			std::size_t target_size = size_ + len;
			if (target_size > capacity_)
			{
				if (capacity_ == 0)
					capacity_ = 1;
				while (target_size > capacity_)
					capacity_ <<= 1;
				resize();
			}

			bit_access::copy_set(buf_, size_, r, i, len);
			size_ = target_size;
		}

		template<typename T>
		void
		append(T v, std::size_t len) //CINDEX_WARN_UNUSED_RESULT
		{
			std::size_t target_size = size_ + len;
			if (target_size > capacity_)
			{
				if (capacity_ == 0)
					capacity_ = 1;
				while (target_size > capacity_)
					capacity_ <<= 1;
				resize();
			}

			bit_access::copy_set(buf_, size_, &v, block_info<T>::bits_per_block - len, len);
			size_ = target_size;
		}

		template<typename T>
		T
		get(std::size_t i, std::size_t len) const //CINDEX_WARN_UNUSED_RESULT
		{
			assert(len <= block_info<T>::bits_per_block);

			T v = 0;
			bit_access::copy_set<T, block_type>(&v, block_info<T>::bits_per_block - len, buf_, i, len);
			return v;
		}

		// TODO: append consecutive 0's or 1's for fast exp-golomb coding encoding
		// TODO: count consecutive 0's or 1's starting at some position for faster exp-golomb coding decoding

		void compact();

	protected:
		void resize();

	private:
		block_type* buf_;
		std::size_t size_;
		std::size_t capacity_;
	};
}
