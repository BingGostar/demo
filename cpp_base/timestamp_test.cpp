#include "timestamp.hpp"
#include <string>
#include <chrono>
#include <thread>
#include <time.h>
#include <sys/time.h>
#include <iostream>

using namespace std;
using namespace StBase;

int main() {

    Timestamp tt = Timestamp::Now();
    cout << tt.FormatString() << endl;
    // Duration dd = Duration::Seconds(4);
    // tt += dd;
    // cout << tt.FormatString() << endl;
    // this_thread::sleep_for(Duration::Seconds(4));
    Timestamp tt1 = Timestamp::Now();

    Duration dd1 = tt1 - tt;
    cout << duration_cast<chrono::microseconds>(tt.time_point.time_since_epoch()).count() << endl;
    cout << duration_cast<chrono::microseconds>(tt1.time_point.time_since_epoch()).count() << endl;
    cout << duration_cast<chrono::microseconds>(dd1.duration).count() << endl;


    Timestamp tt2 = Timestamp::Now();
    cout << (tt2 == TIMESTAMP_ZERO) << endl;

}