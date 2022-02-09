#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "hi_unf_cipher.h"

using namespace std;

static const size_t g_block_size = 16;
static const std::string g_key = "1234567812345678";
static const std::string g_iv = g_key;

int main() {
    HI_S32 err = HI_UNF_CIPHER_Init();
    if (err != 0) cout << "HI_UNF_CIPHER_Init err " << err << endl;
    HI_HANDLE phCipher;
    HI_UNF_CIPHER_ATTS_S pstCipherAttr;
    pstCipherAttr.enCipherType = HI_UNF_CIPHER_TYPE_NORMAL;
    err = HI_UNF_CIPHER_CreateHandle(&phCipher, &pstCipherAttr);
    if (err != 0) cout << "HI_UNF_CIPHER_CreateHandle err " << err << endl;

    HI_UNF_CIPHER_CTRL_S ctrl = {0};
    memcpy((void*)&ctrl.u32Key, g_key.c_str(), g_block_size);
    memcpy((void*)&ctrl.u32IV, g_iv.c_str(), g_block_size);
    ctrl.bKeyByCA = HI_FALSE;
    ctrl.enAlg = HI_UNF_CIPHER_ALG_AES;
    ctrl.enBitWidth = HI_UNF_CIPHER_BIT_WIDTH_128BIT;
    ctrl.enWorkMode = HI_UNF_CIPHER_WORK_MODE_CBC;
    ctrl.enKeyLen = HI_UNF_CIPHER_KEY_AES_128BIT;
    // ctrl.stChangeFlags = 
    HI_UNF_CIPHER_ConfigHandle(phCipher, &ctrl);
}