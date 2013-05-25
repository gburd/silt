/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef _SILT_MONITOR_H_
#define _SILT_MONITOR_H_

#include "silt_proxy.h"
#include "rate_limiter.h"
#include <pthread.h>
#include <tbb/atomic.h>

namespace silt {

    // configuration
    //   <type>: "monitor" (fixed)
    //   <(others)>: used by the main store

    class Silt_Monitor : public Silt_Proxy {
    public:
        Silt_Monitor();
        virtual ~Silt_Monitor();

        virtual Silt_Return Create();
        virtual Silt_Return Open();

        virtual Silt_Return Close();

        virtual Silt_Return Put(const ConstValue& key, const ConstValue& data);
        virtual Silt_Return Append(Value& key, const ConstValue& data);

        virtual Silt_Return Delete(const ConstValue& key);

        virtual Silt_Return Contains(const ConstValue& key) const;
        virtual Silt_Return Length(const ConstValue& key, size_t& len) const;
        virtual Silt_Return Get(const ConstValue& key, Value& data, size_t offset = 0, size_t len = -1) const;

    protected:
        static void* thread_main(void* arg);

    private:
        pthread_t tid_;

        volatile bool exiting_;

        mutable tbb::atomic<size_t> write_ops_;
        mutable tbb::atomic<size_t> read_ops_;

        uint64_t last_time_;

        RateLimiter rate_limiter_;
    };

} // namespace silt

#endif  // #ifndef _SILT_MONITOR_H_
