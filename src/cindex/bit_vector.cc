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

#include "bit_vector.h"
#include <cstring>

namespace cindex
{
	template<typename BlockType>
	bit_vector<BlockType>::bit_vector()
		: buf_(NULL), size_(0), capacity_(8)
	{
		resize();
	}

	template<typename BlockType>
	bit_vector<BlockType>::~bit_vector()
	{
		clear();
	}

	template<typename BlockType>
	void
	bit_vector<BlockType>::clear()
	{
		free(buf_);
		buf_ = NULL;

		size_ = 0;
		capacity_ = 0;
	}

	template<typename BlockType>
	void
	bit_vector<BlockType>::compact()
	{
		capacity_ = size_;
		resize();
	}

	template<typename BlockType>
	void
	bit_vector<BlockType>::resize()
	{
		std::size_t old_byte_size = block_info<block_type>::size(size_);
		std::size_t new_byte_size = block_info<block_type>::size(capacity_);

		block_type* new_buf = reinterpret_cast<block_type*>(realloc(reinterpret_cast<void*>(buf_), new_byte_size));

		if (!new_buf)
		{
			assert(buf_);
			assert(new_byte_size > old_byte_size);

			new_buf = reinterpret_cast<block_type*>(malloc(new_byte_size));
			assert(new_buf);

			memcpy(new_buf, buf_, old_byte_size);

			free(buf_);
		}

		buf_ = new_buf;

		if (new_byte_size > old_byte_size)
			memset(reinterpret_cast<uint8_t*>(new_buf) + old_byte_size, 0, new_byte_size - old_byte_size);
	}

	template class bit_vector<uint8_t>;
	template class bit_vector<uint16_t>;
	template class bit_vector<uint32_t>;
	template class bit_vector<uint64_t>;
}
