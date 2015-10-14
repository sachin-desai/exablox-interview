#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mt_config.h"
#include "merkletree.h"
#include "mt_arr_list.h"
#include "usr_args.h"
#include "utils.h"
#include "floppies.h"
#include "hash.h"

#define ROUNDUP_DIV(x,y)    (x / y + (x % y != 0))

/* 
 * wr_meta_floppy 
 *	Create metadata floppy  	
 *
 * input: 
 * 	fm: file metadata (read from floppy.meta)
 *
 * output:
 * 	ret: fail status if unable to open destination meta file
 */
static int
wr_meta_floppy(mt_t* mt, f_meta* fm, const char* meta_file)
{
	FILE* fp = NULL;
	int ret = 0;
	mt_hash_t mt_root;

	fm->meta_info.idx = 0;
	fm->meta_info.total_shards =
		ROUNDUP_DIV(fm->meta_info.file_sz, SHARD_DATA_SZ);

	fp = fopen(meta_file, "w");
	if (!fp) {
		error_print("Unable to open metadata file '%s'\n", meta_file);
		ret = 1;
		goto error;
	}

	fwrite(&fm->meta_info.idx, sizeof(fm->meta_info.idx), 1, fp);  
	fwrite(&fm->meta_info.file_sz, sizeof(fm->meta_info.file_sz), 1, fp);
	fwrite(&fm->meta_info.total_shards,
		sizeof(fm->meta_info.total_shards), 1, fp);  
	//fwrite(fm->meta_info.sha, sizeof(fm->meta_info.sha), 1, fp);  
	mt_get_root(mt, mt_root);
	fwrite(mt_root, SHA256_DIGEST_LENGTH, 1, fp);  

	printf("Please mail the following %lu floppies to the receiver:\n",
		fm->meta_info.total_shards + 1);
	if (fm->meta_info.total_shards == 1)
		printf("-- %s\n-- floppy.1\n", meta_file);
	else
		printf("-- %s\n-- floppy.1 to floppy.%lu\n",
			meta_file, fm->meta_info.total_shards);
		
	debug_print("WR %s, file size bytes %lu, total shards %lu\n",
		meta_file, fm->meta_info.file_sz, fm->meta_info.total_shards);

error:
	if (fp)
		fclose(fp);
	
	return ret;
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
 * 	ret: fail ret if unable to open destination shard floppy file
 */
static int
write_shard_floppy(mt_t* mt, f_shard* fs, unsigned long shard_bytes)
{
	FILE* fp = NULL;
	SHA256_CTX ctx;
	int ret = 0;
	unsigned zeroes = 0;
	char shard_file[64] = {0};

	hash_init(&ctx);
	hash_update(&ctx, (unsigned char*) fs->shard_info.data, shard_bytes);
	hash_final(fs->shard_info.sha, &ctx);

	sprintf(shard_file, "floppy.%lu", fs->shard_info.idx);

	fp = fopen(shard_file, "w");
	if (!fp) {
		error_print("Unable to open shard floppy '%s'\n", shard_file);
		ret = 1;
		goto error;
	}

	fwrite(&fs->shard_info.idx, sizeof(fs->shard_info.idx), 1, fp);  
	fwrite(fs->shard_info.data, shard_bytes, 1, fp);  
	fwrite(&zeroes, SHARD_DATA_SZ - shard_bytes, 1, fp);
	fwrite(fs->shard_info.sha, sizeof(fs->shard_info.sha), 1, fp);  

	mt_add(mt, fs->shard_info.sha, SHA256_DIGEST_LENGTH);

	//hash_print(shard_file, fs->shard_info.sha);

	debug_print("WR floppy.%lu, shard bytes %zu, total bytes %zu\n",
		fs->shard_info.idx, shard_bytes, sizeof(fs->shard_blob));
	
error:
	if (fp)
		fclose(fp);

	return ret;

}

/* 
 * create_floppies
 *	Create metadata and data floppies from original file
 *
 * input: 
 * 	src_file: original file to shard and create into individual floppies
 *
 * output:
 * 	ret: fail status if unable to open source file
 */
int
create_floppies(usr_args* args)
{
	mt_t* mt = NULL;
	//SHA256_CTX meta_ctx;
	FILE *fp = NULL;
	int ret = 0;
	unsigned long i = 1;
	unsigned long shard_bytes = 0;
	struct floppy_meta fm;
	struct floppy_shard fs;

	memset(&fm, 0, sizeof(struct floppy_meta));
	memset(&fs, 0, sizeof(struct floppy_shard));

	mt = mt_create();
	if (access(args->src_file, F_OK) < 0) {
		error_print("Unable to access file '%s'\n", args->src_file);
		ret = 1;
		goto error;
	}
		
	//hash_init(&meta_ctx);	
	fp = fopen(args->src_file, "r");
	if (!fp) {
		error_print("Unable to open file '%s'\n", args->src_file);
		ret = 1;
		goto error;
	}

	/* shard input file and  calculate sha */
	while ((shard_bytes =
		fread(fs.shard_info.data, 1, sizeof(fs.shard_info.data), fp))) {
			fs.shard_info.idx = i++;
			fm.meta_info.file_sz += shard_bytes;
			//hash_update(&meta_ctx, fs.shard_info.data, shard_bytes);
			ret = write_shard_floppy(mt, &fs, shard_bytes);
			if (ret)
				goto error;
	}

	//hash_final(fm.meta_info.sha, &meta_ctx);
	wr_meta_floppy(mt, &fm, args->meta_file);

	mt_delete(mt);

error:
	if (fp)
		fclose(fp);

	return ret;
}
