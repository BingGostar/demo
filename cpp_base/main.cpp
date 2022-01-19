#include "file_utils.hpp"
#include "timestamp.hpp"
#include "lockfree_snowflake.hpp"

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
#include <map>
#include <unordered_map>

using namespace std;
using namespace StBase;

int main() {

	int threads = 5;

	std::vector<std::thread> vec;
	std::vector<std::map<int, int>> mm;
	// mm.reserve(30);
	std::mutex mut;
	for (size_t i=0; i < threads; i++ ){
		std::map<int, int> tm;
		mm.push_back(tm);
		
		vec.push_back(std::thread([&mm, i](){
			// std::this_thread::sleep_for(std::chrono::seconds(2));
			
			cout << "capacity " << mm.capacity() << endl;
			std::map<int, int>& tmp1 = mm[i];

			

			for (int j = 0; j < 200; j++) {
				tmp1[j] = j;
			}
		}));
	}


	for (size_t i = 0; i< threads; i++) {
		vec[i].join();
	}
	
	cout << "end" << endl;
	
	for (int i : vector<int>({1,2,3,4})) {
		cout << i << endl;
	}

}