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

#include "bit_access.h"

namespace cindex
{
	/*
#define CINDEX_SHIFT(base)	\
	1ull << ((base) + 7), 1ull << ((base) + 6), 1ull << ((base) + 5), 1ull << ((base) + 4), 1ull << ((base) + 3), 1ull << ((base) + 2), 1ull << ((base) + 1), 1ull << ((base) + 0),

	template<>
	const uint8_t bit_table<uint8_t>::bit_table_[block_info<uint8_t>::bits_per_block] =
	{
		CINDEX_SHIFT(0)
	};

	template<>
	const uint16_t bit_table<uint16_t>::bit_table_[block_info<uint16_t>::bits_per_block] =
	{
		CINDEX_SHIFT(8)
		CINDEX_SHIFT(0)
	};

	template<>
	const uint32_t bit_table<uint32_t>::bit_table_[block_info<uint32_t>::bits_per_block] =
	{
		CINDEX_SHIFT(24)
		CINDEX_SHIFT(16)
		CINDEX_SHIFT(8)
		CINDEX_SHIFT(0)
	};

	template<>
	const uint64_t bit_table<uint64_t>::bit_table_[block_info<uint64_t>::bits_per_block] =
	{
		CINDEX_SHIFT(56)
		CINDEX_SHIFT(48)
		CINDEX_SHIFT(40)
		CINDEX_SHIFT(32)
		CINDEX_SHIFT(24)
		CINDEX_SHIFT(16)
		CINDEX_SHIFT(8)
		CINDEX_SHIFT(0)
	};

#undef CINDEX_SHIFT
	*/
}
