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

#include "file_io.h"

#ifdef __APPLE__
namespace silt {

	// non-atomic emulation of preadv and pwritev

	ssize_t
	preadv(int fd, const struct iovec *iovec, int count, off_t offset)
	{
		ssize_t total_read_bytes = 0;
		for (int i = 0; i < count; i++) {
			ssize_t read_bytes = pread(fd, iovec[i].iov_base, iovec[i].iov_len, offset);
			if (read_bytes < 0)
				return read_bytes;
			if (static_cast<size_t>(read_bytes) < iovec[i].iov_len)
				break;
			total_read_bytes += read_bytes;
			offset += iovec[i].iov_len;
		}
		return total_read_bytes;
	}

	ssize_t
	pwritev(int fd, const struct iovec *iovec, int count, off_t offset)
	{
		ssize_t total_written_bytes = 0;
		for (int i = 0; i < count; i++) {
			ssize_t written_bytes = pwrite(fd, iovec[i].iov_base, iovec[i].iov_len, offset);
			if (written_bytes < 0)
				return written_bytes;
			if (static_cast<size_t>(written_bytes) < iovec[i].iov_len)
				break;
			total_written_bytes += written_bytes;
			offset += iovec[i].iov_len;
		}
		return total_written_bytes;
	}

}
#endif
