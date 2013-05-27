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
#include <algorithm>
//#include <iostream>	// for debug

namespace cindex
{
	// example internal representation for uint8_t blocks
	//
	// block index:           0        1        2        3
	// in-block offset:   76543210 76543210 76543210 76543210
	//                   +--------+--------+--------+--------+
	// blocks:           |01101011|00011010|10111010|110     |
	//                   +--------+--------+--------+--------+
	// bit_access index:  0          1          2
	//                    01234567 89012345 67890123 456
	//
	// rationale of filling MSB first (despite of complexity in calculating in-block offsets):
	//   the whole representation of blocks resembles a big-endian number (if the last block is completely filled),
	//   so appending or extracting a block of bits is more natural.

	/*
	template<typename BlockType>
	class bit_table
	{
	public:
		static BlockType
		get(std::size_t i) CINDEX_WARN_UNUSED_RESULT
		{
			return bit_table_[i];
		}

	private:
		static const BlockType bit_table_[block_info<BlockType>::bits_per_block];
	};
	*/

	class bit_access
	{
	public:
		template<typename BlockType>
		static std::size_t
		block_index(std::size_t i)
		{
			return i >> block_info<BlockType>::log_bits_per_block;
		}

		template<typename BlockType>
		static std::size_t
		in_block_offset(std::size_t i)
		{
			return i & block_info<BlockType>::bit_mask;
		}

		// single bit operations
		template<typename BlockType>
		static void
		set(BlockType* v, std::size_t i)
		{
			v[block_index<BlockType>(i)] |= static_cast<BlockType>(BlockType(1) << (block_info<BlockType>::bits_per_block - 1 - in_block_offset<BlockType>(i)));
			//v[block_index<BlockType>(i)] |= bit_table<BlockType>::get(in_block_offset<BlockType>(i));
		}

		template<typename BlockType>
		static void
		unset(BlockType* v, std::size_t i)
		{
			v[block_index<BlockType>(i)] &= static_cast<BlockType>(~(BlockType(1) << (block_info<BlockType>::bits_per_block - 1 - in_block_offset<BlockType>(i))));
			//v[block_index<BlockType>(i)] &= static_cast<BlockType>(~bit_table<BlockType>::get(in_block_offset<BlockType>(i)));
		}

		template<typename BlockType>
		static void
		flip(BlockType* v, std::size_t i)
		{
			v[block_index<BlockType>(i)] ^= static_cast<BlockType>(BlockType(1) << (block_info<BlockType>::bits_per_block - 1 - in_block_offset<BlockType>(i)));
			//v[block_index<BlockType>(i)] ^= bit_table<BlockType>::get(in_block_offset<BlockType>(i));
		}

		template<typename BlockType>
		static bool
		get(const BlockType* v, std::size_t i) //CINDEX_WARN_UNUSED_RESULT
		{
			return (v[block_index<BlockType>(i)] & (BlockType(1) << (block_info<BlockType>::bits_per_block - 1 - in_block_offset<BlockType>(i)))) != 0;
		}

		// multiple bits operations
		template<typename DestBlockType, typename SrcBlockType>
		static void
		copy_set(DestBlockType* dest, std::size_t dest_i, const SrcBlockType* src, std::size_t src_i, std::size_t count)
		{
			// get dest_off and src_off in each block's range
			dest += block_index<DestBlockType>(dest_i);
			src += block_index<SrcBlockType>(src_i);
			dest_i = in_block_offset<DestBlockType>(dest_i);
			src_i = in_block_offset<SrcBlockType>(src_i);

			while (count != 0)
			{
				// the remaining free space in the dest block
				std::size_t dest_available = block_info<DestBlockType>::bits_per_block - dest_i;
				// the remaining bits in the src block
				std::size_t src_available = block_info<SrcBlockType>::bits_per_block - src_i;
				// the actual copiable bits
				std::size_t copy_len = std::min(std::min(dest_available, src_available), count);

				// fetch a block from the source
				SrcBlockType src_v = *src;
				//std::cout << "1: " << src_v << std::endl;
				// trim off the higher bits before the range being copied and align bits at MSB
				src_v = static_cast<SrcBlockType>(src_v << src_i);
				//std::cout << "2: " << src_v << std::endl;
				// trim off the lower bits after the range being copied and align bits at LSB
				src_v = static_cast<SrcBlockType>(src_v >> (block_info<SrcBlockType>::bits_per_block - copy_len));
				//std::cout << "3: " << src_v << std::endl;
				// copy the value to the dest-like block
				DestBlockType dest_v = static_cast<DestBlockType>(src_v);
				assert(static_cast<SrcBlockType>(dest_v) == src_v);
				//std::cout << "4: " << dest_v << std::endl;
				// rearrange bits for the dest block
				dest_v = static_cast<DestBlockType>(dest_v << (block_info<DestBlockType>::bits_per_block - dest_i - copy_len));
				//std::cout << "5: " << dest_v << std::endl;
				// merge to the dest block (this does not clear out the existing bits at the dest)
				*dest |= dest_v;
				//std::cout << "6: " << dest << std::endl;

				// update the current block locations
				if (dest_available == copy_len)
				{
					dest++;
					dest_i = 0;
				}
				else
					dest_i += copy_len;
				if (src_available == copy_len)
				{
					src++;
					src_i = 0;
				}
				else
					src_i += copy_len;
				count -= copy_len;
			}
		}
	};
}
