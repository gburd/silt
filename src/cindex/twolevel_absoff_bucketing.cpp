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

#include "twolevel_absoff_bucketing.hpp"
#include <iostream>

namespace cindex
{
	template<typename ValueType, typename UpperValueType>
	twolevel_absoff_bucketing<ValueType, UpperValueType>::twolevel_absoff_bucketing(std::size_t size, std::size_t keys_per_bucket, std::size_t keys_per_block)
	{
		resize(size, keys_per_bucket, keys_per_block);
	}

	template<typename ValueType, typename UpperValueType>
	void
	twolevel_absoff_bucketing<ValueType, UpperValueType>::resize(std::size_t size, std::size_t keys_per_bucket, std::size_t keys_per_block CINDEX_UNUSED)
	{
		size_ = size;
		keys_per_bucket_ = keys_per_bucket;

		std::size_t bits_per_key_bound = 4;
		upper_bucket_size_ = (std::size_t(1) << (sizeof(ValueType) * 8)) / (bits_per_key_bound * keys_per_bucket_);
		if (upper_bucket_size_ == 0)
			upper_bucket_size_ = 1;

		bucket_info_.resize(size);
		upper_bucket_info_.resize(size / upper_bucket_size_ + 2);
		upper_bucket_info_[0][0] = 0;
		upper_bucket_info_[0][1] = 0;
		current_i_ = 0;

		//if (size_)
		//{
		//	std::cout << "bucket_count: " << size << std::endl;
		//	std::cout << "upper_bucket_count: " << size / upper_bucket_size_ + 2 << std::endl;
		//}
	}

	template<typename ValueType, typename UpperValueType>
	void
	twolevel_absoff_bucketing<ValueType, UpperValueType>::insert(const std::size_t& index_offset, const std::size_t& dest_offset)
	{
		assert(current_i_ < size_);

		std::size_t base_index_offset = upper_index_offset(current_i_ / upper_bucket_size_);
		std::size_t base_dest_offset = upper_dest_offset(current_i_ / upper_bucket_size_);

		//std::cout << "i: " << current_i_ << std::endl;
		//std::cout << "base: " << base_index_offset << " " << base_dest_offset << std::endl;
		//std::cout << "current: " << index_offset << " " << dest_offset << std::endl;

		bucket_info_[current_i_][0] = guarded_cast<value_type>(index_offset - base_index_offset);
		bucket_info_[current_i_][1] = guarded_cast<value_type>(dest_offset - base_dest_offset);
		upper_bucket_info_[current_i_ / upper_bucket_size_ + 1][0] = guarded_cast<upper_value_type>(index_offset);
		upper_bucket_info_[current_i_ / upper_bucket_size_ + 1][1] = guarded_cast<upper_value_type>(dest_offset);

		assert(this->index_offset(current_i_) == index_offset);
		assert(this->dest_offset(current_i_) == dest_offset);

		current_i_++;
	}

	template<typename ValueType, typename UpperValueType>
	std::size_t
	twolevel_absoff_bucketing<ValueType, UpperValueType>::index_offset(std::size_t i) const
	{
		assert(i < size_);
		return upper_index_offset(i / upper_bucket_size_) + bucket_info_[i][0];
	}

	template<typename ValueType, typename UpperValueType>
	std::size_t
	twolevel_absoff_bucketing<ValueType, UpperValueType>::dest_offset(std::size_t i) const
	{
		assert(i < size_);
		return upper_dest_offset(i / upper_bucket_size_) + bucket_info_[i][1];
	}

	template<typename ValueType, typename UpperValueType>
	std::size_t
	twolevel_absoff_bucketing<ValueType, UpperValueType>::upper_index_offset(std::size_t i) const
	{
		assert(i < size_ / upper_bucket_size_ + 1);
		return upper_bucket_info_[i][0];
	}

	template<typename ValueType, typename UpperValueType>
	std::size_t
	twolevel_absoff_bucketing<ValueType, UpperValueType>::upper_dest_offset(std::size_t i) const
	{
		assert(i < size_ / upper_bucket_size_ + 1);
		return upper_bucket_info_[i][1];
	}

	template<typename ValueType, typename UpperValueType>
	std::size_t
	twolevel_absoff_bucketing<ValueType, UpperValueType>::size() const
	{
		return size_;
	}

	template<typename ValueType, typename UpperValueType>
	std::size_t
	twolevel_absoff_bucketing<ValueType, UpperValueType>::bit_size() const
	{
		std::size_t bit_size = bucket_info_.size() * 2 * sizeof(value_type) * 8;
		bit_size += upper_bucket_info_.size() * 2 * sizeof(upper_value_type) * 8;
		return bit_size;
	}

	template class twolevel_absoff_bucketing<>;
}
