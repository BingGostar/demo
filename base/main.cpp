#include "file_utils.hpp"
#include "encode.hpp"
#include <iostream>
#include <string.h>
#include <vector>
#include <string>

using namespace std;
using namespace StBase;

int main() {
    const char* buf = "dadadadadasasg";
    int r = FileUtils::Write("aa.txt", buf, strlen(buf));
    cout << r << endl;
    std::vector<char> bb;
    FileUtils::ReadFile("aa.txt", std::move(bb));
    cout << string(bb.data(), bb.size()) << endl;

    cout << Encode::Base64Encode(buf, strlen(buf)) << endl;
    cout << Encode::CheckMd5(buf, strlen(buf)) << endl;
}