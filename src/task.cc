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

#include "task.h"
#include "debug.h"
#include <cassert>

#include <stdio.h>
#include <errno.h>

#ifdef __linux__

#include <sys/types.h>
#include <bits/syscall.h>

extern "C" {
pid_t gettid()
{
    return (pid_t)syscall(SYS_gettid);
}
}

extern "C" {
// from linux/ioprio.h

#define IOPRIO_BITS             (16)
#define IOPRIO_CLASS_SHIFT      (13)
#define IOPRIO_PRIO_MASK        ((1UL << IOPRIO_CLASS_SHIFT) - 1)

#define IOPRIO_PRIO_CLASS(mask) ((mask) >> IOPRIO_CLASS_SHIFT)
#define IOPRIO_PRIO_DATA(mask)  ((mask) & IOPRIO_PRIO_MASK)
#define IOPRIO_PRIO_VALUE(class, data)  (((class) << IOPRIO_CLASS_SHIFT) | data)

#define ioprio_valid(mask)      (IOPRIO_PRIO_CLASS((mask)) != IOPRIO_CLASS_NONE)

enum {
    IOPRIO_CLASS_NONE,
    IOPRIO_CLASS_RT,
    IOPRIO_CLASS_BE,
    IOPRIO_CLASS_IDLE,
};

enum {
    IOPRIO_WHO_PROCESS = 1,
    IOPRIO_WHO_PGRP,
    IOPRIO_WHO_USER,
};

static inline int ioprio_set(int which, int who, int ioprio)
{
    return syscall(__NR_ioprio_set, which, who, ioprio);
}

static inline int ioprio_get(int which, int who)
{
    return syscall(__NR_ioprio_get, which, who);
}

}
#endif


namespace silt {
    TaskScheduler::TaskScheduler(size_t num_workers, size_t queue_capacity, cpu_priority_t cpu_priority, io_priority_t io_priority)
        : cpu_priority_(cpu_priority), io_priority_(io_priority)
    {
        assert(num_workers > 0);

        shared_queue_.set_capacity(queue_capacity);

        for (size_t i = 0; i < num_workers; i++) {
            Worker* worker = new Worker();
            worker->owner = this;
            worker->seen_join = false;
            worker->queue = &shared_queue_;
            //worker->queue.set_capacity(queue_capacity);

            if (pthread_create(&worker->tid, NULL, Worker::thread_main, worker) != 0) {
                DPRINTF(2, "TaskScheduler::TaskScheduler(): failed to create worker\n");
                break;
            }

            workers_.push_back(worker);
        }

        //next_worker_ = 0;
    }

    TaskScheduler::~TaskScheduler()
    {
        if (workers_.size())
            join();
    }

    void
    TaskScheduler::enqueue_task(Task* t)
    {
        //workers_[next_worker_++ % workers_.size()]->queue.push(t);
        shared_queue_.push(t);
    }

    void
    TaskScheduler::join()
    {
        for (size_t i = 0; i < workers_.size(); i++) {
            JoinTask* t = new JoinTask();
            //workers_[i]->queue.push(t);
            shared_queue_.push(t);
        }

        for (size_t i = 0; i < workers_.size(); i++) {
            pthread_join(workers_[i]->tid, NULL);
            delete workers_[i];
            workers_[i] = NULL;
        }

        workers_.clear();
    }

    void*
    TaskScheduler::Worker::thread_main(void* arg)
    {
        Worker* this_worker = reinterpret_cast<Worker*>(arg);

#ifdef __linux__
        pid_t tid = gettid();

        {
            if (this_worker->owner->cpu_priority_ == CPU_LOW) {
                nice(1);
                // TODO: log int new_nice = nice(1);
                //fprintf(stderr, "set nice() = %d\n", new_nice);
            }
        }

        {
            // from Documentation/block/ioprio.txt
            unsigned int ioprio = 4;
            int ioprio_class = IOPRIO_CLASS_BE;

            switch (this_worker->owner->io_priority_) {
            case IO_REAL_TIME:
                ioprio_class = IOPRIO_CLASS_RT;
                break;
            case IO_BEST_EFFORT:
                ioprio_class = IOPRIO_CLASS_BE;
                break;
            case IO_IDLE:
                ioprio_class = IOPRIO_CLASS_IDLE;
                ioprio = 7;
                break;
            default:
                assert(false);
                break;
            }

            int ret = ioprio_set(IOPRIO_WHO_PROCESS, tid, IOPRIO_PRIO_VALUE(ioprio_class, ioprio));
            if (ret) {
                perror("Error while setting I/O priority");
                assert(false);
            }
            assert(IOPRIO_PRIO_CLASS(ioprio_get(IOPRIO_WHO_PROCESS, tid)) == ioprio_class);
            assert(IOPRIO_PRIO_DATA(ioprio_get(IOPRIO_WHO_PROCESS, tid)) == ioprio);
        }
#endif

        while (!this_worker->seen_join) {
            Task* t;
            this_worker->queue->pop(t);
            t->seen_join = &this_worker->seen_join;
            t->Run();
            delete t;
        }
        return NULL;
    }

    void
    TaskScheduler::JoinTask::Run()
    {
        *seen_join = true;
    }

} // namespace silt
