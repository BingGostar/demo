#ifndef __STBASE_CRYPTO__
#define __STBASE_CRYPTO__

#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include "openssl/aes.h"
#include "openssl/sha.h"

#include "exception.hpp"
#include "encode.hpp"

namespace StBase {

class Crypto {
public:
	static void Init(const std::string & aesKey);
	static std::string AesEncrypt(const char *data, size_t len);
	static std::string AesEncrypt(const std::string &data);
	static void AesEncrypt(const char *, size_t, std::vector<char> &);
	static void AesEncrypt(const std::string &, std::vector<char> &);

	// aes 解密
	static std::string AesDecrypt(const std::string &data);
	static void AesDecrypt(const char *, size_t, std::vector<char> &);
	static void AesDecrypt(const std::string &, std::vector<char> &);
	// static void AesDecrypt(const char *, size_t, std::string &);
	// static void AesDecrypt(const char *, size_t, int &);
	// static void AesDecrypt(const char *, size_t, float &);

private:
	// (key对齐128)
	static void sha128(std::vector<unsigned char> & res, const std::string & key);
	static void sha256(std::vector<unsigned char> & res, const std::string & key);

	static void pkcs5_padding(std::vector<char> &, size_t);
	static void pkcs5_unpadding(std::vector<char> &);

private:
	static const size_t key_size = 16;
	static AES_KEY aes_key_en;
	static AES_KEY aes_key_de;
};


// 初始向量
static const unsigned char g_iv[AES_BLOCK_SIZE] = { 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x2C,
                                                    0x77, 0x6F, 0x72, 0x6C, 0x64, 0x6C,
                                                    0x6F, 0x2C, 0x77, 0x6F };

AES_KEY Crypto::aes_key_en = {0};
AES_KEY Crypto::aes_key_de = {0};

void Crypto::Init(const std::string & keyStr) {
	if (keyStr.size() == 0) throw Exception("key len 0");
	std::vector<unsigned char> buf;
    sha128(buf, keyStr);
	if (AES_set_decrypt_key(buf.data(), buf.size() * 8, &aes_key_de) < 0) {
		throw Exception("AES_set_decrypt_key error ");
	}
	if (AES_set_encrypt_key(buf.data(), buf.size() * 8, &aes_key_en) < 0) {
		throw Exception("AES_set_encrypt_key error ");
	}
}

void Crypto::AesEncrypt(const char *data, size_t len, std::vector<char> &dst) {
	if (len == 0) return;
	std::vector<char> oridata(data, data + len);
	pkcs5_padding(oridata, AES_BLOCK_SIZE);
	dst.resize(oridata.size());
	unsigned char iv[AES_BLOCK_SIZE];
	memcpy(iv, g_iv, AES_BLOCK_SIZE);
	AES_cbc_encrypt((unsigned char *)oridata.data(), (unsigned char *)dst.data(), oridata.size(), &aes_key_en, iv, AES_ENCRYPT);
}

void Crypto::AesEncrypt(const std::string &data, std::vector<char> &dst) {
	AesEncrypt(data.c_str(), data.size(), dst);
}

std::string Crypto::AesEncrypt(const char *data, size_t len) {
	std::string res;
	if (len == 0) return res;
	std::vector<char> oridata(data, data + len);
	pkcs5_padding(oridata, AES_BLOCK_SIZE);
	std::vector<unsigned char> encryData(oridata.size());

	unsigned char iv[AES_BLOCK_SIZE];
	memcpy(iv, g_iv, AES_BLOCK_SIZE);
	AES_cbc_encrypt((const unsigned char *)oridata.data(), (unsigned char *)encryData.data(), oridata.size(), &aes_key_en, iv, AES_ENCRYPT);
	res = Encode::Base64Encode((const char *)encryData.data(), encryData.size());
	return res;
}

std::string Crypto::AesEncrypt(const std::string &data) {
	return AesEncrypt(data.c_str(), data.size());
}

void Crypto::AesDecrypt(const char *data, size_t len, std::vector<char> &dst) {
	if (len == 0) return;
	dst.resize(len);
	unsigned char iv[AES_BLOCK_SIZE];
	memcpy(iv, g_iv, AES_BLOCK_SIZE);
	AES_cbc_encrypt((const unsigned char *)data, (unsigned char *)dst.data(), len, &aes_key_de, iv, AES_DECRYPT);
	pkcs5_unpadding(dst);
}

void Crypto::AesDecrypt(const std::string &data, std::vector<char> &dst) {
	AesDecrypt(data.c_str(), data.size(), dst);
}

// void CryptoUtils::AesDecrypt(const char * data, size_t len, std::string & dst) {
//     std::vector<char> dstvec;
//     AesDecrypt(data, len, dstvec);
//     dst = std::string(dstvec.begin(), dstvec.end());
// }

// void CryptoUtils::AesDecrypt(const char * data, size_t len, int & dst) {
//     std::vector<char> dstvec;
//     AesDecrypt(data, len, dstvec);
//     dst = *(int*)dstvec.data();
// }

// void CryptoUtils::AesDecrypt(const char * data, size_t len, float & dst) {
//     std::vector<char> dstvec;
//     AesDecrypt(data, len, dstvec);
//     dst = *(float*)dstvec.data();
// }

std::string Crypto::AesDecrypt(const std::string &data) {

	std::string res;
	if (data.size() == 0) return res;
	std::string decStrbB64 = Encode::Base64Decode(data.c_str(), data.size());
	std::vector<char> decVec(decStrbB64.begin(), decStrbB64.end());
	std::vector<char> decryData(decVec.size());
	unsigned char iv[AES_BLOCK_SIZE];
	memcpy(iv, g_iv, AES_BLOCK_SIZE);
	AES_cbc_encrypt((const unsigned char *)decVec.data(), (unsigned char *)decryData.data(), decVec.size(), &aes_key_de, iv, AES_DECRYPT);
    pkcs5_unpadding(decryData);
	res.assign(decryData.begin(), decryData.end());
    return res;
}


void Crypto::pkcs5_padding(std::vector<char> &data, size_t blockSize) {
	size_t padding = blockSize - data.size() % blockSize;
	for (size_t i = 0; i < padding; i++) {
		data.push_back(static_cast<char>(padding));
	}
}
void Crypto::pkcs5_unpadding(std::vector<char> &data) {
	if (data.size() == 0) return;
	size_t padding = static_cast<size_t>(data[data.size() - 1]);
	if (data.size() < padding) return;
    data.resize(data.size() - padding);
}

void Crypto::sha128(std::vector<unsigned char> & res, const std::string & key) {
	res.resize(16);
	SHA_CTX sha128;
	SHA1_Init(&sha128);
	SHA1_Update(&sha128, key.c_str(), key.size());
	SHA1_Final(res.data(), &sha128);
}

void Crypto::sha256(std::vector<unsigned char> & res, const std::string & key) {
	res.resize(32);
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, key.c_str(), key.size());
	SHA256_Final(res.data(), &sha256);
}






}
#endif