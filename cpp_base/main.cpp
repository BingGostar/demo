#include "file_utils.hpp"
#include "timestamp.hpp"
#include "uid_snowflake.hpp"

#include "encode.hpp"
#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <chrono>
#include <stdint.h>
#include <thread>
#include <set>
#include <mutex>
#include <unordered_map>

using namespace std;
using namespace StBase;

int main() {

	int threads = 20;

	std::vector<std::thread> vec;
	std::vector<std::unordered_map<int64_t, char>> mm(threads);
	std::mutex mut;
	for (size_t i=0; i < threads; i++ ){
		// std::unordered_map<int64_t, char> tmp;
		// mm.push_back(tmp);
		vec.emplace_back(std::thread([&mm, i](){
			std::unordered_map<int64_t, char>& tmp1 = mm[i];
			std::stringstream ss;
			ss << std::this_thread::get_id();
			// printf("thread [%s] id [%ld] tmp size [%ld]\n", ss.str().c_str(), i, tmp1.bucket_count());
			for (size_t j = 0; j < 200000; j++) {
				int64_t a = SnowFlake::NextId();
				if (tmp1.find(a) != tmp1.end()) {
					printf("thread [%s] id [%ld]\n", ss.str().c_str(), a);
				} else {
					tmp1[a] = 0;
				}
			}
		}));
	}

	std::unordered_map<int64_t, int> mstore;

	for (size_t i = 0; i< threads; i++) {
		vec[i].join();
		for (auto it = mm[i].begin(); it != mm[i].end(); it++) {
			if (mstore.find(it->first) == mstore.end()) {
				mstore[it->first] = i;
			} else {
				printf("i [%ld] id [%ld] old id [%d]\n",i, it->first, mstore[it->first]);
				SnowFlake::ParsePrintId(it->first);
			}
		}
	}
	
	cout << "end" << endl;
	

}