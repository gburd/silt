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

#include "silt_monitor.h"
#include <stdio.h>
#include <sys/time.h>
#ifndef __APPLE__
#include <sys/sysinfo.h>
#endif

namespace silt {

    Silt_Monitor::Silt_Monitor()
        : exiting_(false),
          rate_limiter_(1, 1, 1, 1000000000L)    // 1s report interval
    {
        write_ops_ = 0;
        read_ops_ = 0;
    }

    Silt_Monitor::~Silt_Monitor()
    {
        // dup -- Silt_Proxy calls its Close() in destructor
        if (store_)
            Close();
    }

    Silt_Return
    Silt_Monitor::Create()
    {
        Silt_Return ret = this->Silt_Proxy::Create();

        if (store_) {
            struct timeval tv;

            if (gettimeofday(&tv, NULL)) {
                perror("Error while getting the current time");
            }

            last_time_ = static_cast<uint64_t>(tv.tv_sec) * 1000000000Lu + static_cast<uint64_t>(tv.tv_usec) * 1000Lu;

            if (pthread_create(&tid_, NULL, thread_main, this)) {
                perror("Error while creating a monitor thread");
            }
        }

        return ret;
    }

    Silt_Return
    Silt_Monitor::Open()
    {
        Silt_Return ret = this->Silt_Proxy::Open();

        if (store_) {
            struct timeval tv;

            if (gettimeofday(&tv, NULL)) {
                perror("Error while getting the current time");
            }

            last_time_ = static_cast<uint64_t>(tv.tv_sec) * 1000000000Lu + static_cast<uint64_t>(tv.tv_usec) * 1000Lu;

            if (pthread_create(&tid_, NULL, thread_main, this)) {
                perror("Error while creating a monitor thread");
            }
        }

        return ret;
    }

    Silt_Return
    Silt_Monitor::Close()
    {
        if (store_) {
            // this has a race condition issue but should be find if usually only one thread calls Close()
            exiting_ = true;

            //if (pthread_cancel(tid_)) {
            //    perror("Error while canceling the monitor thread");
            //}
            if (pthread_join(tid_, NULL)) {
                perror("Error while stopping the monitor thread");
            }
        }

        return this->Silt_Proxy::Close();
    }


    Silt_Return
    Silt_Monitor::Put(const ConstValue &key, const ConstValue &data)
    {
        ++write_ops_;
        return this->Silt_Proxy::Put(key, data);
    }

    Silt_Return
    Silt_Monitor::Append(Value &key, const ConstValue &data)
    {
        ++write_ops_;
        return this->Silt_Proxy::Append(key, data);
    }

    Silt_Return
    Silt_Monitor::Delete(const ConstValue &key)
    {
        ++write_ops_;
        return this->Silt_Proxy::Delete(key);
    }

    Silt_Return
    Silt_Monitor::Contains(const ConstValue &key) const
    {
        ++read_ops_;
        return this->Silt_Proxy::Contains(key);
    }

    Silt_Return
    Silt_Monitor::Length(const ConstValue &key, size_t &len) const
    {
        ++read_ops_;
        return this->Silt_Proxy::Length(key, len);
    }

    Silt_Return
    Silt_Monitor::Get(const ConstValue &key, Value &data, size_t offset, size_t len) const
    {
        ++read_ops_;
        return this->Silt_Proxy::Get(key, data, offset, len);
    }

    void *
    Silt_Monitor::thread_main(void *arg)
    {
        Silt_Monitor *monitor = reinterpret_cast<Silt_Monitor *>(arg);

        while (!monitor->exiting_) {
            monitor->rate_limiter_.remove_tokens(1);
            size_t read_ops = monitor->read_ops_;
            monitor->read_ops_ -= read_ops;
            size_t write_ops = monitor->write_ops_;
            monitor->write_ops_ -= write_ops;
            struct timeval tv;

            if (gettimeofday(&tv, NULL)) {
                perror("Error while getting the current time");
            }

            uint64_t current_time = static_cast<uint64_t>(tv.tv_sec) * 1000000000Lu + static_cast<uint64_t>(tv.tv_usec) * 1000Lu;
            double time_diff = static_cast<double>(current_time - monitor->last_time_) / 1000000000.;
            monitor->last_time_ = current_time;
            size_t rmem_size = 0;
#ifndef __APPLE__
            FILE *fp = fopen("/proc/self/statm", "r");

            if (fp) {
                unsigned long vmem;
                unsigned long rmem = 0;

                if (fscanf(fp, "%lu %lu", &vmem, &rmem) == 2) {
                    rmem_size = rmem * getpagesize();
                }

                fclose(fp);
            }

#endif
            Value status_num_active_data;
            monitor->Status(NUM_ACTIVE_DATA, status_num_active_data);
            Value status_memory_use;
            monitor->Status(MEMORY_USE, status_memory_use);
            fprintf(stdout, "%llu.%06llu: %9.2lf reads/s, %9.2lf writes/s, %7.2lf MB total\n",
                    static_cast<long long unsigned>(tv.tv_sec),
                    static_cast<long long unsigned>(tv.tv_usec),
                    static_cast<double>(read_ops) / time_diff,
                    static_cast<double>(write_ops) / time_diff,
                    static_cast<double>(rmem_size) / 1000000.
                   );
            fprintf(stdout, "%llu.%06llu: NUM_ACTIVE_DATA %s\n",
                    static_cast<long long unsigned>(tv.tv_sec),
                    static_cast<long long unsigned>(tv.tv_usec),
                    status_num_active_data.str().c_str()
                   );
            fprintf(stdout, "%llu.%06llu: MEMORY_USE %s\n",
                    static_cast<long long unsigned>(tv.tv_sec),
                    static_cast<long long unsigned>(tv.tv_usec),
                    status_memory_use.str().c_str()
                   );
            fflush(stdout);
        }

        return NULL;
    }

} // namespace silt
