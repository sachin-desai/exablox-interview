#ifndef __FLOPPY_H__
#define __FLOPPY_H__

#include <openssl/sha.h>

#define SHARD_DATA_SZ 	 984

typedef struct floppy_meta {
	union {
		struct meta_info {
			unsigned long idx; /* must be zero */
			unsigned long file_sz;
			unsigned long total_shards;
			unsigned char sha[SHA256_DIGEST_LENGTH];
		} meta_info;
		unsigned char meta_blob[sizeof(struct meta_info)];	
	};
} f_meta;

typedef struct floppy_shard {
	union {
		struct shard_info {
			unsigned long idx;
			unsigned char data[SHARD_DATA_SZ];
			unsigned char sha[SHA256_DIGEST_LENGTH];
		} shard_info;
		unsigned char shard_blob[sizeof(struct shard_info)];
	};
} f_shard;

#endif
