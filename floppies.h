#ifndef __FLOPPY_H__
#define __FLOPPY_H__

#include <openssl/sha.h>

#define OUTPUT_FILE_NAME "final.file"
#define META_FILE_NAME	"floppy.meta"
#define SHARD_DATA_SZ 	 984 /* debug size */

struct floppy_meta {
	union {
		struct meta_info {
			unsigned long idx; /* must be zero */
			unsigned long file_sz;
			unsigned long total_shards;
			unsigned char sha[SHA256_DIGEST_LENGTH];
		} meta_info;
		unsigned char meta_blob[sizeof(struct meta_info)];	
	};
};

struct floppy_shard {
	union {
		struct shard_info {
			unsigned long idx;
			unsigned char data[SHARD_DATA_SZ];
			unsigned char sha[SHA256_DIGEST_LENGTH];
		} shard_info;
		unsigned char shard_blob[sizeof(struct shard_info)];
	};
};

#endif
