#include <stdio.h>
#include <openssl/sha.h>
#include "hash.h"
#include "utils.h"

/* 
 * hash_print
 *  	Debug print function for sha checksum 
 *
 * input: 
 * 	msg: user supplied string
 * 	sha: checksum output
 *
 * output:
 * 	void
 */
void
hash_print(const char* msg, unsigned char* sha)
{
	int i;

	printf("%s: ", msg);
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
		printf("%02X", sha[i]);
	printf("\n");
}

/* 
 * hash_init
 *  	Hash context initializer; wrapper provided to allow different
 *  	checksum in future
 *
 * input: 
 * 	sha256 ctx: Context to track checksum
 *
 * output:
 * 	void
 */
void
hash_init(SHA256_CTX* ctx)
{
	SHA256_Init(ctx);
}

/* 
 * hash_update
 *  	Hash context update; wrapper provided to allow different
 *  	checksum in future
 *
 * input: 
 * 	ctx: Context to track checksum
 * 	input: data for checksum 
 * 	len: data length for checksum
 *
 * output:
 * 	void
 */
void
hash_update(SHA256_CTX* ctx, void* input, unsigned long len)
{
	SHA256_Update(ctx, (unsigned char*) input, len);
}

/* 
 * hash_final
 *  	Hash completion; wrapper provided to allow different
 *  	checksum in future
 *
 * input: 
 * 	sha: output checksum bytes
 * 	ctx: Context to track checksum
 *
 * output:
 * 	void
 */
void
hash_final(unsigned char* sha, SHA256_CTX* ctx)
{
	SHA256_Final(sha, ctx);
}
