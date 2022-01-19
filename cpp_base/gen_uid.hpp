#ifndef __STBASE_UID_SNOWFLAKE__
#define __STBASE_UID_SNOWFLAKE__

// lock-free snowflake generate uid algorithm
// snowflake: https://github.com/twitter-archive/snowflake

#include <stdint.h>
#include <chrono>
#include <atomic>
#include <assert.h>


namespace StBase
{

template<typename T>
using time_point = std::chrono::time_point<T>;
using system_clock = std::chrono::system_clock;
using steady_clock = std::chrono::steady_clock;

class SnowFlake {
public:
    // 起始时间，机器号
    static int64_t start_stamp;
    static int machine_id;
    // 每一部分占的位数
    static const int sequence_bits = 12;
    static const int machine_bits = 5;
    static const int timestamp_bits = 41;
    // 每一部分的MAX
    static const int64_t MAX_SEQUENCE = (1L << sequence_bits) - 1;
    static const int64_t MAX_MACHINE = (1L << machine_bits) - 1;
    static const int64_t MAX_TIMESTAMP = (1L << timestamp_bits) - 1;
    // 移位
    static const int MACHINE_LEFT = sequence_bits;
    static const int TIMESTAMP_LEFT = machine_bits + sequence_bits;

    static void Init(int64_t startStamp, int machineId) {
        
    }

    static int64_t NextId() {
        int64_t curStamp;
        int64_t seq;

        do {
            int64_t localSeq = sequence_.load();
            int64_t localLast = last_timestamp_.load();
            curStamp = millSecond();
            seq = 0;

            if (curStamp < localLast) {
                continue;
            }

            if (curStamp == localLast) {
                seq = MAX_SEQUENCE & (localSeq + 1);
                if (seq == 0) { 
                    continue;
                }
            }

            assert(seq <= MAX_SEQUENCE);
            
            if (last_timestamp_.compare_exchange_weak(localLast, curStamp) && 
                sequence_.compare_exchange_weak(localSeq, seq)) {
                    break;
            }

        } while(1);

        assert(curStamp - start_stamp <= MAX_TIMESTAMP);

        return ((curStamp - start_stamp) << TIMESTAMP_LEFT) |
                (machine_id << MACHINE_LEFT) |
                seq;
    }

    // static std::string ParseId(int64_t id) {
    //     int64_t timestamp = id >> (sequence_bits + machine_bits);
    //     int64_t sequence = id & MAX_SEQUENCE;
    //     // std::cout << "time " << timestamp << " sequence " << sequence << std::endl;

    // }

private:
    static int64_t millSecond() {
        int64_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now() - start_time_point_).count();
        return start_mill_ + diff;
    }

private:
    static int64_t start_mill_;
    static time_point<steady_clock> start_time_point_;
    // 序列号
    static std::atomic<int64_t> sequence_;
    // 最新时间戳
    static std::atomic<int64_t> last_timestamp_;
};

int64_t SnowFlake::start_mill_ = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
time_point<steady_clock> SnowFlake::start_time_point_ = steady_clock::now();
std::atomic<int64_t> SnowFlake::last_timestamp_(-1);
std::atomic<int64_t> SnowFlake::sequence_(0);
int64_t SnowFlake::start_stamp = 1480166465631L;
int SnowFlake::machine_id = 0;


}

#endif