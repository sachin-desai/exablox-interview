#ifndef __HASH_H__
#define __HASH_H__

#include <openssl/sha.h>

void hash_init(SHA256_CTX* ctx);
void hash_update(SHA256_CTX* ctx, void* data, unsigned long len);
void hash_final(unsigned char* sha, SHA256_CTX* ctx);
void hash_print(const char * msg, unsigned char* md);

#endif
