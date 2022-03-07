#include <chrono>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <chrono>

namespace StBase {

using namespace std::chrono;

struct Duration
{
    typedef std::chrono::microseconds MicroSeconds;
    typedef std::chrono::milliseconds MilliSeconds;
    typedef std::chrono::seconds Seconds;
    typedef std::chrono::minutes Minutes;
    typedef std::chrono::hours Hours;
    typedef std::chrono::duration<int64_t, std::ratio<60 * 60 * 24>> Days;

    Duration(const system_clock::duration & dur):duration(dur) {}
    
    template<typename rp, typename pd>
    Duration(const std::chrono::duration<rp, pd> & dur):duration(dur) {}

    system_clock::duration duration;
};


class Timestamp
{
    public:
    static const int64_t MicroSecondsPerSecond = 1000 * 1000; // 以微秒为单位

    Timestamp() = default;
    Timestamp(const Timestamp &) = default;
    Timestamp(Timestamp &&) = default;
    Timestamp &operator=(const Timestamp &stamp) {
        if (this != &stamp) {
            this->time_point = stamp.time_point;
        }
        return *this;
    }

    explicit Timestamp(const system_clock::duration & dur) : time_point(dur) {}
    explicit Timestamp(const system_clock::time_point & tp) : time_point(tp) {}

    static Timestamp Now() {
        Timestamp stamp;
        stamp.time_point = system_clock::now();
        return stamp;
    }

    static Timestamp FromUnixTime(time_t tt) {
        Timestamp stamp;
        stamp.time_point = system_clock::from_time_t(tt);
        return stamp;
    }

    static Timestamp FromUnixTime(time_t tt, int mics) {
        Timestamp stamp(std::chrono::microseconds(tt * MicroSecondsPerSecond + mics));
        return stamp;
    }

    static Timestamp FromString(const std::string &str) {
        int year, mon, day, hour, min, sec;
        ::sscanf(str.c_str(), "%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec);
        struct tm t = { 0 };
        t.tm_year = year - 1900;
        t.tm_mon = mon - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = min;
        t.tm_sec = sec;
        return Timestamp::FromUnixTime(timegm(&t));
    }

    static Timestamp FromDayTime(int year, int mon, int day, int hour = 0, int min = 0,
                                 int sec = 0) {
        ::tm t = { 0 };
        t.tm_year = year - 1900;
        t.tm_mon = mon - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = min;
        t.tm_sec = sec;
        return Timestamp::FromUnixTime(timegm(&t));
    }

    public:
    ::time_t ToUnixTime() const {
        return system_clock::to_time_t(time_point);
    }

    ::tm ToTmTime() const {
        time_t tt_time = system_clock::to_time_t(time_point);
        struct tm tm_time = { 0 };
        gmtime_r(&tt_time, &tm_time);
        return tm_time;
    }

    std::string FormatString() const {
        time_t tt_time = system_clock::to_time_t(time_point);
        struct tm tm_time = { 0 };
        gmtime_r(&tt_time, &tm_time);
        char buf[64] = { 0 };
        snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d", tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min,
                 tm_time.tm_sec);
        return buf;
    }

    friend bool operator<(const Timestamp &ltp, const Timestamp &rtp) {
        return ltp.time_point < rtp.time_point;
    }

    friend bool operator>(const Timestamp &ltp, const Timestamp &rtp) {
        return ltp.time_point > rtp.time_point;
    }

    friend bool operator==(const Timestamp &ltp, const Timestamp &rtp) {
        return ltp.time_point == rtp.time_point;
    }

    friend bool operator!=(const Timestamp &ltp, const Timestamp &rtp) {
        return ltp.time_point != rtp.time_point;
    }

    friend Duration operator-(const Timestamp& ltp, const Timestamp& rtp) {
        return ltp.time_point - rtp.time_point;
    }


    Timestamp operator-(const Duration &dur) {
        return Timestamp(this->time_point - dur.duration);
    }

    Timestamp& operator-=(const Duration &dur) {
        this->time_point -= dur.duration;
        return *this;
    }
    
    Timestamp operator+(const Duration &dur) {
        return Timestamp(this->time_point + dur.duration);
    }

    Timestamp& operator+=(const Duration &dur) {
        this->time_point += dur.duration;
        return *this;
    }

    public:
    system_clock::time_point time_point;
};


}