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
#include "bit_access.hpp"
#include "huffman.hpp"
#include "exp_golomb.hpp"
#include "sign_interleave.hpp"
//#include <iostream>

namespace cindex
{
	template<bool WeakOrdering = false, unsigned int HuffmanCodingLimit = 16, typename RefType = uint8_t>
	class trie
	{
	public:
		trie()
		{
			// prepare huffman coding for n <= HuffmanCodingLimit
			for (unsigned int n = 2; n <= HuffmanCodingLimit; n++)
			{
				if (!WeakOrdering)
				{
					huffman_tree_generator<uint64_t> gen(n + 1);

					uint64_t v = 1;
					gen[0] = v;
					for (unsigned int k = 1; k <= n; k++)
						gen[k] = v = v * (n - k + 1) / k;

					huffman_tree<RefType> t(n + 1);
					gen.generate(t);

					huff_[n - 2] = new huffman<RefType>(t);
				}
				else
				{
					huffman_tree_generator<uint64_t> gen(n);

					uint64_t v = 1;
					gen[0] = v * 2;
					for (unsigned int k = 1; k <= n - 1; k++)
						gen[k] = v = v * (n - k + 1) / k;

					huffman_tree<RefType> t(n);
					gen.generate(t);

					huff_[n - 2] = new huffman<RefType>(t);
				}
			}
		}

		template<typename DistType>
		void
		recreate_huffman_from_dist(DistType& dist)
		{
			assert(!WeakOrdering);

			for (unsigned int n = 2; n <= HuffmanCodingLimit; n++)
				delete huff_[n - 2];

			for (unsigned int n = 2; n <= HuffmanCodingLimit; n++)
			{
				huffman_tree_generator<uint64_t> gen(n + 1);

				for (unsigned int k = 0; k <= n; k++)
					gen[k] = dist[n][k];

				huffman_tree<RefType> t(n + 1);
				gen.generate(t);

				huff_[n - 2] = new huffman<RefType>(t);
			}
		}

		virtual ~trie()
		{
			for (unsigned int n = 2; n <= HuffmanCodingLimit; n++)
			{
				delete huff_[n - 2];
				huff_[n - 2] = NULL;
			}
		}

		template<typename Buffer, typename KeyArrayType>
		void encode(Buffer& out_buf, const KeyArrayType& arr, std::size_t key_len, std::size_t off, std::size_t n, std::size_t dest_base = 0, std::size_t dest_keys_per_block = 1, std::size_t skip_bits = 0) const
		{
			encode_rec(out_buf, arr, key_len, off, n, dest_base, dest_keys_per_block, skip_bits);
		}

		template<typename Buffer>
		std::size_t locate(const Buffer& in_buf, std::size_t& in_out_buf_iter, const uint8_t* key, std::size_t key_len, std::size_t off, std::size_t n, std::size_t dest_base = 0, std::size_t dest_keys_per_block = 1, std::size_t skip_bits = 0) const
		{
			return locate_rec(in_buf, in_out_buf_iter, key, key_len, off, n, dest_base, dest_keys_per_block, skip_bits);
		}

	protected:
		template<typename Buffer, typename KeyArrayType>
		void
		encode_rec(Buffer& out_buf, const KeyArrayType& arr, std::size_t key_len, std::size_t off, std::size_t n, std::size_t dest_base, std::size_t dest_keys_per_block, std::size_t depth) const
		{
			//std::cout << "encode: " << off << " " << n << " " << depth << std::endl;

			// do not encode 0- or 1-sized trees
			/*
			if (n == 1 || depth >= 4)
			{
				for (std::size_t k = depth; k < 4; k++)
					out_buf.push_back(0);
				return;
			}
			*/
			if (n <= 1)
				return;

			// k-perfect hashing
			if (n <= dest_keys_per_block && (dest_base + off) / dest_keys_per_block == (dest_base + off + n - 1) / dest_keys_per_block)
				return;

			assert(depth < key_len * 8);    // duplicate key?

			// find the number of keys on the left tree
			std::size_t left = 0;
			for (; left < n; left++)
			{
				if (bit_access::get(arr[off + left], depth))   // assume sorted keys
					break;
			}

			// replace (n, 0) split with (0, n) split if weak ordering is used
			if (WeakOrdering && left == n)
				left = 0;

			// encode the left tree size
			if (n <= HuffmanCodingLimit)
				huff_[n - 2]->encode(out_buf, left);
			else
				exp_golomb<>::encode<std::size_t>(out_buf, sign_interleave::encode<std::size_t>(left - n / 2));

			encode_rec(out_buf, arr, key_len, off, left, dest_base, dest_keys_per_block, depth + 1);
			encode_rec(out_buf, arr, key_len, off + left, n - left, dest_base, dest_keys_per_block, depth + 1);
		}

		template<typename Buffer>
		std::size_t
		locate_rec(const Buffer& in_buf, std::size_t& in_out_buf_iter, const uint8_t* key, std::size_t key_len, std::size_t off, std::size_t n, std::size_t dest_base, std::size_t dest_keys_per_block, std::size_t depth) const
		{
			//std::cout << "locate: " << off << " " << n << " " << depth << std::endl;

			// do not encode 0- or 1-sized trees
			if (n <= 1)
				return 0;

			// k-perfect hashing
			if (n <= dest_keys_per_block && (dest_base + off) / dest_keys_per_block == (dest_base + off + n - 1) / dest_keys_per_block)
				return 0;

			assert(depth < key_len * 8);    // invalid code?

			// decode the left tree size
			std::size_t left;
			if (n <= HuffmanCodingLimit)
				left = huff_[n - 2]->decode(in_buf, in_out_buf_iter);
			else
				left = sign_interleave::decode<std::size_t>(exp_golomb<>::decode<std::size_t>(in_buf, in_out_buf_iter)) + n / 2;
			assert(left <= n);

			// find the number of keys on the left to the key (considering weak ordering)
			if (!bit_access::get(key, depth) && (!WeakOrdering || (WeakOrdering && left != 0)))
			{
				return locate_rec(in_buf, in_out_buf_iter, key, key_len, off, left, dest_base, dest_keys_per_block, depth + 1);
			}
			else
			{
				skip_rec(in_buf, in_out_buf_iter, key, key_len, off, left, dest_base, dest_keys_per_block, depth + 1);
				return left + locate_rec(in_buf, in_out_buf_iter, key, key_len, off + left, n - left, dest_base, dest_keys_per_block, depth + 1);
			}
		}

		template<typename Buffer>
		void
		skip_rec(const Buffer& in_buf, std::size_t& in_out_buf_iter, const uint8_t* key, std::size_t key_len, std::size_t off, std::size_t n, std::size_t dest_base, std::size_t dest_keys_per_block, std::size_t depth) const
		{
			//std::cout << "skip: " << off << " " << n << " " << depth << std::endl;

			// do not encode 0- or 1-sized trees
			if (n <= 1)
				return;

			// k-perfect hashing
			if (n <= dest_keys_per_block && (dest_base + off) / dest_keys_per_block == (dest_base + off + n - 1) / dest_keys_per_block)
				return;

			assert(depth < key_len * 8);    // invalid code?

			// decode the left tree size
			std::size_t left;
			if (n <= HuffmanCodingLimit)
				left = huff_[n - 2]->decode(in_buf, in_out_buf_iter);
			else
				left = sign_interleave::decode<std::size_t>(exp_golomb<>::decode<std::size_t>(in_buf, in_out_buf_iter)) + n / 2;
			assert(left <= n);

			skip_rec(in_buf, in_out_buf_iter, key, key_len, off, left, dest_base, dest_keys_per_block, depth + 1);
			skip_rec(in_buf, in_out_buf_iter, key, key_len, off + left, n - left, dest_base, dest_keys_per_block, depth + 1);
		}

	protected:
		huffman<RefType>* huff_[HuffmanCodingLimit - 1];
	};
}
