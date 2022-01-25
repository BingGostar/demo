#include "crypto.hpp"
// #include "encode.hpp"
#include <iostream>
#include <string>


using namespace StBase;
using namespace std;

void test1() {
    Crypto::Init("adsadadw  dsada");

    for(size_t i=0;i < 10000; i++) {
        std::string a = Encode::GenRandStr(1000);
        std::string en = Crypto::AesEncrypt(a);
        std::string r = Crypto::AesDecrypt(en);
        if (a != r) cout << "error " << a << endl;
    }

}

void test2() {
    Crypto::Init("adsadadw  dsada");
    int fd = ::open("aaa.jpg", O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        cout << "open read file error" << endl;
        return;
    }
    int dstfd = ::open("aaa.jpg.cipher", O_RDWR | O_CREAT | O_CLOEXEC, 0664);
    if (dstfd < 0) {
        cout << "open write file error" << endl;
        return;
    }
    Crypto::AesEncrypt(fd, dstfd);
    ::close(fd);
    ::close(dstfd);

    // ========================================================
    fd = ::open("aaa.jpg.cipher", O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        cout << "open read cipher file error" << endl;
        return;
    }
    dstfd = ::open("aaa1.jpg", O_RDWR | O_CREAT | O_CLOEXEC, 0664);
    if (dstfd < 0) {
        cout << "open write file error" << endl;
        return;
    }
    Crypto::AesDecrypt(fd, dstfd);
    ::close(fd);
    ::close(dstfd);
}

int main(){
    
    test2();

}