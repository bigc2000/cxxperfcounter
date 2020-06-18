#pragma  once

#include <atomic>
#include <cstdint>

#define INT64_T std::int64_t
#define  INT32_T std::int32_t
#define INT16_T std::int16_t
#define INT8_T std::int8_t

namespace mc {

typedef std::atomic_uint64_t AtomicULong;
typedef std::atomic_int64_t AtomicLong;
typedef std::atomic_uint AtomicUInteger;
typedef std::atomic_int AtomicInteger;
typedef std::atomic<double> AtomicDouble;
typedef std::atomic<bool> AtomicBoolean;

//    typedef std::atomic<INT64_T> AtomicLong;
//    typedef std::atomic<INT32_T> AtomicInteger;
}