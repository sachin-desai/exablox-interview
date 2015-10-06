#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "utils.h"
#include "floppies.h"
#include "hash.h"

#define ROUNDUP_DIV(x,y)    (x / y + (x % y != 0))

/* 
 * write_meta_floppy 
 *	Create metadata floppy  	
 *
 * input: 
 * 	fm: file metadata (read from floppy.meta)
 *
 * output:
 * 	bool: fail status if unable to open destination meta file
 */
static bool
write_meta_floppy(struct floppy_meta* fm)
{
	FILE* fp = NULL;
	bool status = true;

	fm->meta_info.idx = 0;
	fm->meta_info.total_shards =
		ROUNDUP_DIV(fm->meta_info.file_sz, SHARD_DATA_SZ);

	fp = fopen(META_FILE_NAME, "w");
	if (!fp) {
		printf("Unable to open metadata %s file\n", META_FILE_NAME);
		status = false;
		goto error;
	}

	fwrite(&fm->meta_info.idx, sizeof(fm->meta_info.idx), 1, fp);  
	fwrite(&fm->meta_info.file_sz, sizeof(fm->meta_info.file_sz), 1, fp);
	fwrite(&fm->meta_info.total_shards,
		sizeof(fm->meta_info.total_shards), 1, fp);  
	fwrite(fm->meta_info.sha, sizeof(fm->meta_info.sha), 1, fp);  

	if (fm->meta_info.total_shards == 1) {
		printf("Please mail the following floppy to the receiver:\n");
		printf("floppy.1\n");
	} else {
		printf("Please mail the following floppies to the receiver:\n"); 
		printf("floppy.1 to floppy.%lu\n", fm->meta_info.total_shards);
	}
		
	debug_print("WR %s, file size bytes %lu, total shards %lu\n",
		META_FILE_NAME, fm->meta_info.file_sz, fm->meta_info.total_shards); 

error:
	if (fp)
		fclose(fp);
	
	return status;
}

/* 
 * write_shard_floppy 
 *	Create floppy from original sharded data. File naming is "floppy.X".
 *
 * input: 
 * 	fs: floppy data
 * 	shard_bytes: amount to write 
 *
 * output:
 * 	bool: fail status if unable to open destination shard floppy file
 */
static bool
write_shard_floppy(struct floppy_shard* fs, unsigned long shard_bytes)
{
	FILE* fp = NULL;
	SHA256_CTX ctx;
	bool status = true;
	unsigned zeroes = 0;
	char shard_file[64] = {0};

	hash_init(&ctx);
	hash_update(&ctx, (unsigned char*) fs->shard_info.data, shard_bytes);
	hash_final(fs->shard_info.sha, &ctx);

	sprintf(shard_file, "floppy.%lu", fs->shard_info.idx);

	fp = fopen(shard_file, "w");
	if (!fp) {
		printf("Unable to open shard floppy '%s'\n", shard_file);
		status = false;
		goto error;
	}

	fwrite(&fs->shard_info.idx, sizeof(fs->shard_info.idx), 1, fp);  
	fwrite(fs->shard_info.data, shard_bytes, 1, fp);  
	fwrite(&zeroes, SHARD_DATA_SZ - shard_bytes, 1, fp);
	fwrite(fs->shard_info.sha, sizeof(fs->shard_info.sha), 1, fp);  

	debug_print("WR floppy.%lu, shard bytes %zu, total bytes %zu\n",
		fs->shard_info.idx, shard_bytes, sizeof(fs->shard_blob));
	
error:
	if (fp)
		fclose(fp);

	return status;

}

/* 
 * create_floppies
 *	Create metadata and data floppies from original file
 *
 * input: 
 * 	src_file: original file to shard and create into individual floppies
 *
 * output:
 * 	bool: fail status if unable to open source file
 */
bool
create_floppies(const char* src_file)
{
	SHA256_CTX ctx;
	FILE *fp = NULL;
	bool status = true;
	unsigned long i = 1;
	unsigned long shard_bytes = 0;
	struct floppy_meta fm;
	struct floppy_shard fs;

	if (access(src_file, F_OK) < 0) {
		printf("Invalid source file '%s' to convert into floppies\n", src_file);
		status = false;
		goto error;
	}
		
	hash_init(&ctx);	
	fp = fopen(src_file, "r");
	if (!fp) {
		printf("Unable to open source file '%s'\n", src_file);
		status = false;
		goto error;
	}

	memset(&fm, 0, sizeof(struct floppy_meta));
	memset(&fs, 0, sizeof(struct floppy_shard));

	/* shard input file and  calculate sha */
	while ((shard_bytes =
		fread(fs.shard_info.data, 1, sizeof(fs.shard_info.data), fp))) {
			fs.shard_info.idx = i++;
			fm.meta_info.file_sz += shard_bytes;
			hash_update(&ctx, fs.shard_info.data, shard_bytes);
			status = write_shard_floppy(&fs, shard_bytes);
			if (status == false)
				goto error;
	}

	hash_final(fm.meta_info.sha, &ctx);
	write_meta_floppy(&fm);

error:
	if (fp)
		fclose(fp);

	return status;
}
