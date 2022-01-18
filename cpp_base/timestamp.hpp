#include <chrono>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

namespace StBase {

// std::chrono::nanoseconds    duration<long long, std::ratio<1, 1000000000>>
// std::chrono::microseconds   duration<long long, std::ratio<1, 1000000>>
// std::chrono::milliseconds   duration<long long, std::ratio<1, 1000>>
// std::chrono::seconds        duration<long long>
// std::chrono::minutes        duration<int, std::ratio<60>>
// std::chrono::hours

class Timestamp {
public:
    static const int MicroSecondsPerSecond = 1000 * 1000;

    Timestamp() = default;

    explicit Timestamp(int64_t t):microSec_(t) {}

    static Timestamp Now() {
        struct timeval val;
        gettimeofday(&val, NULL);
        return Timestamp(val.tv_sec * MicroSecondsPerSecond  + val.tv_usec);
    }

    // static Timestamp FromString() {

    // }

    static Timestamp FromUnixTime(time_t tt) {
        return FromUnixTime(tt, 0);
    }

    static Timestamp FromUnixTime(time_t tt, int mics) {
        return Timestamp(tt * MicroSecondsPerSecond + mics);
    }

    static Timestamp FromString(const std::string & str) {
        int year, mon, day, hour, min, sec;
        ::sscanf(str.c_str(), "%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
        struct tm t = {0};
        t.tm_year = year - 1900;
        t.tm_mon = mon - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = min;
        t.tm_sec = sec;
        
        return FromUnixTime(timegm(&t));
    }

public:
    time_t ToUnixTime() const {
        return static_cast<time_t>(microSec_ / MicroSecondsPerSecond);
    }

    std::string FormatString() const {
        time_t tt_time = static_cast<time_t>(microSec_ / MicroSecondsPerSecond);
        struct tm tm_time = {0};
        gmtime_r(&tt_time, &tm_time);
        char buf[64] = {0};
        snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d", 
                    tm_time.tm_year + 1900,
                    tm_time.tm_mon + 1,
                    tm_time.tm_mday,
                    tm_time.tm_hour,
                    tm_time.tm_min,
                    tm_time.tm_sec);
        return buf;
    }

    friend bool operator<(const Timestamp & ltp, const Timestamp & rtp) {
        return ltp.microSec_ < rtp.microSec_;
    }

    friend bool operator==(const Timestamp & ltp, const Timestamp & rtp) {
        return ltp.microSec_ == rtp.microSec_;
    }

    friend bool operator!=(const Timestamp & ltp, const Timestamp & rtp) {
        return ltp.microSec_ != rtp.microSec_;
    }

private:
    int64_t microSec_;

};


}