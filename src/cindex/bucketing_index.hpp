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

#include "common.hpp"
#include "bit_vector.hpp"
#include "key_array.hpp"
#include "trie.hpp"
#include <vector>

namespace cindex
{
	template<typename BucketingType>
	class bucketing_index
	{
	public:
		typedef BucketingType bucketing_type;

	public:
		bucketing_index(std::size_t key_len, std::size_t n, std::size_t bucket_size, std::size_t dest_base = 0, std::size_t dest_keys_per_block = 1, std::size_t skip_bits = 0);
		bucketing_index(int fd, off_t offset);
		~bucketing_index();

		bool finalized() const CINDEX_WARN_UNUSED_RESULT;

		bool insert(const uint8_t* key);
		void flush();

		ssize_t store_to_file(int fd, off_t offset) CINDEX_WARN_UNUSED_RESULT;

		std::size_t locate(const uint8_t* key) const CINDEX_WARN_UNUSED_RESULT;

		std::size_t bit_size_trie_only() const CINDEX_WARN_UNUSED_RESULT;
		std::size_t bit_size() const CINDEX_WARN_UNUSED_RESULT;

	protected:
		std::size_t find_bucket(const uint8_t* key) const CINDEX_WARN_UNUSED_RESULT;
		void index_pending_keys();

		ssize_t load_from_file(int fd, off_t offset) CINDEX_WARN_UNUSED_RESULT;

		typedef bit_vector<> vector_type;
		typedef key_array key_array_type;

	private:
		std::size_t key_len_;
		std::size_t n_;

		std::size_t bucket_size_;

		std::size_t dest_base_;
		std::size_t dest_keys_per_block_;
		std::size_t skip_bits_;

		std::size_t bucket_count_;
		std::size_t bucket_bits_;

        vector_type repr_;
		trie<> trie_;

		bucketing_type bucketing_;

		std::size_t last_dest_offset_;
		std::size_t pending_bucket_;
		//key_array_type* pending_keys_;
		std::vector<const uint8_t*> pending_keys_;
		std::size_t pending_key_count_;
	};
}
