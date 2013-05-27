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

#include "expected_size.h"
#include <iostream>

namespace cindex
{
	static struct
	{
		std::size_t keys_per_bucket;
		std::size_t keys_per_block;
		bool weak_ranking;
		double bits_per_key;
	} known_index_sizes[] = {
		{1, 	1,	false,	0.000000},
		{2, 	1,	false,	1.500000},
		{4, 	1,	false,	2.035714},
		{8, 	1,	false,	2.368062},
		{16, 	1,	false,	2.565474},
		{32, 	1,	false,	2.719423},
		{64, 	1,	false,	2.800122},
		{128, 	1,	false,	2.846470},
		{256, 	1,	false,	2.872846},
		{512, 	1,	false,	2.887720},
		{1024,	1,	false,	2.896029},

		{1, 	1,	true,	0.000000},
		{2, 	1,	true,	1.000000},
		{4, 	1,	true,	1.595238},
		{8, 	1,	true,	1.925228},
		{16, 	1,	true,	2.122761},
		{32, 	1,	true,	2.276723},
		{64, 	1,	true,	2.357425},
		{128, 	1,	true,	2.403773},
		{256, 	1,	true,	2.430150},
		{512, 	1,	true,	2.445024},
		{1024,	1,	true,	2.453333},

		{1, 	4,	false,	0.000000},
		{2, 	4,	false,	0.375000},
		{4, 	4,	false,	0.750000},
		{8, 	4,	false,	1.153312},
		{16, 	4,	false,	1.355466},
		{32, 	4,	false,	1.509626},
		{64, 	4,	false,	1.590345},
		{128, 	4,	false,	1.636697},
		{256, 	4,	false,	1.663074},
		{512, 	4,	false,	1.677949},
		{1024,	4,	false,	1.686258},

		{1, 	4,	true,	0.000000},
		{2, 	4,	true,	0.250000},
		{4, 	4,	true,	0.651786},
		{8, 	4,	true,	1.048069},
		{16, 	4,	true,	1.249690},
		{32, 	4,	true,	1.403818},
		{64, 	4,	true,	1.484533},
		{128, 	4,	true,	1.530884},
		{256, 	4,	true,	1.557261},
		{512, 	4,	true,	1.572136},
		{1024,	4,	true,	1.580445},

		{1, 	16,	false,	0.000000},
		{2, 	16,	false,	0.093750},
		{4, 	16,	false,	0.130371},
		{8, 	16,	false,	0.202756},
		{16, 	16,	false,	0.382474},
		{32, 	16,	false,	0.530633},
		{64, 	16,	false,	0.609876},
		{128, 	16,	false,	0.655798},
		{256, 	16,	false,	0.682011},
		{512, 	16,	false,	0.696813},
		{1024,	16,	false,	0.705088},

		{1, 	16,	true,	0.000000},
		{2, 	16,	true,	0.062500},
		{4, 	16,	true,	0.119838},
		{8, 	16,	true,	0.197576},
		{16, 	16,	true,	0.377405},
		{32, 	16,	true,	0.525653},
		{64, 	16,	true,	0.604917},
		{128, 	16,	true,	0.650845},
		{256, 	16,	true,	0.677060},
		{512, 	16,	true,	0.691863},
		{1024,	16,	true,	0.700139},
	};

	double
	expected_size::index_size(std::size_t keys_per_bucket CINDEX_UNUSED, std::size_t keys_per_block CINDEX_UNUSED)
	{
		const bool weak_ranking = false;
		for (std::size_t i = 0; i < sizeof(known_index_sizes) / sizeof(known_index_sizes[0]); i++)
		{
			if (known_index_sizes[i].keys_per_bucket == keys_per_bucket &&
				known_index_sizes[i].keys_per_block == keys_per_block &&
				known_index_sizes[i].weak_ranking == weak_ranking)
				return known_index_sizes[i].bits_per_key;
		}
		std::cerr << "bug: no index size information availble for keys_per_bucket = " << keys_per_bucket << ", keys_per_block = " << keys_per_block << std::endl;
		//assert(false);
		return 3.;
	}
}
