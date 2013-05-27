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

#ifndef _TASK_H_
#define _TASK_H_

#include <pthread.h>
#include <vector>
#include <tbb/concurrent_queue.h>

namespace silt {

    // a simple task scheduler with round-robin per-thread queues; we don't use the TBB task scheduler, which is geared towared computation-oriented tasks

    class Task {
        public:
            virtual ~Task() {}
            virtual void Run() = 0;
            bool *seen_join;
    };

    class TaskScheduler {
        public:
            enum cpu_priority_t {
                CPU_HIGH,
                CPU_NORMAL,
                CPU_LOW,
            };

            enum io_priority_t {
                IO_REAL_TIME,
                IO_BEST_EFFORT,
                IO_IDLE,
            };

            TaskScheduler(size_t num_workers, size_t queue_capacity, cpu_priority_t cpu_priority = CPU_NORMAL, io_priority_t io_priority = IO_BEST_EFFORT);
            ~TaskScheduler();

            void enqueue_task(Task *t);
            void join();

        protected:
            struct Worker {
                TaskScheduler *owner;
                pthread_t tid;
                tbb::concurrent_bounded_queue<Task *> *queue;
                bool seen_join;

                static void *thread_main(void *arg);
            };

            class JoinTask : public Task {
                public:
                    virtual void Run();
            };

        private:
            cpu_priority_t cpu_priority_;
            io_priority_t io_priority_;
            std::vector<Worker *> workers_;
            //tbb::atomic<size_t> next_worker_;
            tbb::concurrent_bounded_queue<Task *> shared_queue_;
    };

} // namespace silt

#endif  // #ifndef _TASK_H_
