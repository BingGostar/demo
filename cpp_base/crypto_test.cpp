#include "crypto.hpp"
// #include "encode.hpp"
#include <iostream>
#include <string>


using namespace StBase;
using namespace std;

int main(){
    Crypto::Init("adsadadw  dsada");

    for(size_t i=0;i < 10000; i++) {
        std::string a = Encode::GenRandStr(1000);
        std::string en = Crypto::AesEncrypt(a);
        std::string r = Crypto::AesDecrypt(en);
        if (a != r) cout << "error " << a << endl;
    }

}