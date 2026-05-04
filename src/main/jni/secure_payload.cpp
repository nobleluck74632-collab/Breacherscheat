#include <jni.h>
#include <android/log.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "secure_payload.h"

#define LOG_TAG "SecurePayload"
#define AES_KEY_SIZE 32
#define AES_BLOCK_SIZE 16

const unsigned char aesKey[AES_KEY_SIZE] = {
    0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70, 0x81,
    0x92, 0xa3, 0xb4, 0xc5, 0xd6, 0xe7, 0xf8, 0x09,
    0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70, 0x81,
    0x92, 0xa3, 0xb4, 0xc5, 0xd6, 0xe7, 0xf8, 0x09
};

std::vector<unsigned char> decryptFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        LOGE("Failed to open payload file");
        return {};
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> encrypted(size);
    file.read((char*)encrypted.data(), size);
    file.close();

    // Extract IV (first 16 bytes)
    unsigned char iv[AES_BLOCK_SIZE];
    memcpy(iv, encrypted.data(), AES_BLOCK_SIZE);

    // Decrypt
    std::vector<unsigned char> decrypted(size - AES_BLOCK_SIZE);
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aesKey, iv);
    int outLen;
    EVP_DecryptUpdate(ctx, decrypted.data(), &outLen, encrypted.data() + AES_BLOCK_SIZE, size - AES_BLOCK_SIZE);
    int finalLen;
    EVP_DecryptFinal_ex(ctx, decrypted.data() + outLen, &finalLen);
    EVP_CIPHER_CTX_free(ctx);

    decrypted.resize(outLen + finalLen);
    return decrypted;
}

void applyUpdate(const std::vector<unsigned char>& data) {
    // In real app: apply encrypted patch to memory or filesystem
    LOGD("Update payload decrypted. Size: %zu bytes", data.size());
    // Example: Overwrite a function in memory
}

extern "C" JNIEXPORT void JNICALL
Java_com_omega_breachers_MainActivity_applySecureUpdate(JNIEnv* env, jobject obj) {
    std::vector<unsigned char> payload = decryptFile("/data/data/com.omega.breachers/files/payload.enc");
    if (!payload.empty()) {
        applyUpdate(payload);
    }
}
