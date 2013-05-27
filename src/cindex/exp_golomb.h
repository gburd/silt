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
#include "bit_access.h"

namespace cindex
{
	template<unsigned int Order = 0>
	class exp_golomb
	{
	public:
		template<typename T, typename BufferType>
		static void encode(BufferType& out_buf, const T& n)
		{
			BOOST_STATIC_ASSERT(sizeof(T) * 8 >= Order);

			T m;
			if (Order)
				m = (n >> Order) + 1;
			else
				m = n + 1;

			int len = 0;
			{
				T p = m;
				while (p)
				{
					len++;
					p >>= 1;
				}
			}

			for (int i = 1; i < len; i++)
				out_buf.push_back(0);

			assert(m >> (len - 1) == 1);
			out_buf.push_back(1);

			//for (int i = len - 2; i >= 0; i--)
			//	out_buf.push_back((m >> i) & 1);
			out_buf.append(m, static_cast<std::size_t>(len - 1));

			if (Order)
			{
				//for (int i = guarded_cast<int>(Order) - 1; i >= 0; i--)
				//	out_buf.push_back((n >> i) & 1);
				out_buf.append(n, static_cast<std::size_t>(Order));
			}
		}

		template<typename T, typename BufferType>
		static T decode(const BufferType& in_buf, std::size_t& in_out_buf_iter)
		{
			BOOST_STATIC_ASSERT(sizeof(T) * 8 >= Order);

			int len = 1;
			while (true)
			{
				assert(in_out_buf_iter < in_buf.size());
				if (in_buf[in_out_buf_iter++])
					break;
				len++;
			}

			T m = guarded_cast<T>(1) << (len - 1);
			//for (int i = len - 2; i >= 0; i--)
			//{
			//	assert(in_out_buf_iter < in_buf.size());
			//	if (in_buf[in_out_buf_iter++])
			//		m |= guarded_cast<T>(1) << i;
			//}
			assert(in_out_buf_iter + static_cast<std::size_t>(len - 1) <= in_buf.size());
			m |= in_buf.template get<T>(in_out_buf_iter, static_cast<std::size_t>(len - 1));	// "template" prefix is used to inform the compiler that in_buf.get is a member template
			in_out_buf_iter += static_cast<std::size_t>(len - 1);

			T n;
			if (Order)
			{
				n = (m - 1) << Order;
				assert(in_out_buf_iter + Order <= in_buf.size());
				//for (int i = guarded_cast<int>(Order) - 1; i >= 0; i--)
				//{
				//	if (in_buf[in_out_buf_iter++])
				//		n |= guarded_cast<T>(1) << i;
				//}
				n |= in_buf.template get<T>(in_out_buf_iter, static_cast<std::size_t>(Order));
				in_out_buf_iter += static_cast<std::size_t>(Order);
			}
			else
				n = m - 1;

			return n;
		}
	};
}
