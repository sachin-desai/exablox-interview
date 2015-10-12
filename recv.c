#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "floppies.h"
#include "recv.h"
#include "hash.h"

/* 
 * read_and_verify_shard_floppy
 *  	Read and verify floppy sha checksum
 *
 * input: 
 * 	fm: file metadata (read from floppy.meta)
 * 	fs: shard data
 * 	shard_file: floppy name to read, and verify
 * 	shard_bytes:  
 *
 * output:
 * 	bool: fail status if unable to read shard floppy, or floppy sha does not match
 */
static bool
read_and_verify_shard_floppy(struct floppy_meta* fm, struct floppy_shard* fs,
	char* shard_file, unsigned long* shard_bytes)
{
	SHA256_CTX ctx;
	FILE* fp = NULL;
	unsigned long last_shard_sz = 0;
	unsigned char sha[SHA256_DIGEST_LENGTH];

	last_shard_sz = fm->meta_info.file_sz % SHARD_DATA_SZ; 

	if (!(fp = fopen(shard_file, "r"))) {
		error_print("Unable to read %s\n", shard_file);
		return false;
	}

	fread(fs->shard_blob, sizeof(fs->shard_blob), 1, fp);
	fclose(fp);

	*shard_bytes = sizeof(fs->shard_info.data);
	if (fs->shard_info.idx == fm->meta_info.total_shards)
		if (last_shard_sz) /* ensure not equal to 984 */
			*shard_bytes = last_shard_sz;

	hash_init(&ctx);
	hash_update(&ctx, (unsigned char*) fs->shard_info.data, *shard_bytes);
	hash_final(sha, &ctx);

	if (memcmp(sha, fs->shard_info.sha, SHA256_DIGEST_LENGTH)) {
		error_print("'%s' is corrupted; possible attack or floppy is damaged\n", shard_file);
		return false;
	}

	return true;
}

/* 
 * process_shard_floppy
 *  	Write data to reconstructed file from non-corrupted floppies
 *
 * input: 
 * 	fm: file metadata (read from floppy.meta)
 * 	shard_file: floppy name to read, and verify
 * 	fo: output (aka final destination) file to write floppy data into 
 *
 * output:
 * 	bool: fail status if unable to read shard floppy, or floppy sha does not match
 */
static bool
process_shard_floppy(struct floppy_meta* fm, char* shard_file, FILE* fo)
{
	long offset = 0;
	bool status = true;
	unsigned long shard_bytes = 0;
	struct floppy_shard fs;

	status = read_and_verify_shard_floppy(fm, &fs, shard_file, &shard_bytes); 
	debug_print("RD %s: shard bytes: %lu\n", shard_file, shard_bytes);
	if (status == false) {
		error_print("Checksum for '%s' does not match\n", shard_file);
		goto error;
	}

	offset = (fs.shard_info.idx-1) * SHARD_DATA_SZ; 
	fseek(fo, offset, SEEK_SET); 
	fwrite(fs.shard_info.data, shard_bytes, 1, fo);


error:
	return status;
}

/* 
 * process_meta_floppy
 *  	Gather metadata information from guaranteed delivery file
 *
 * input: 
 * 	fm: file metadata information to populate
 *
 * output:
 * 	bool: fail status if unable to open meta data file 
 * 	      or floppy index is not zero (meta data index should be 0)
 */
static bool
process_meta_floppy(struct floppy_meta* fm)
{
	bool status = true;
	FILE* fp = NULL;

	fp = fopen(META_FILE_NAME, "r");
	if (!fp) {
		error_print("Unable to open metadata file '%s'\n", META_FILE_NAME);
		status = false;
		goto error;
	}

	fread(fm->meta_blob, sizeof(fm->meta_blob), 1, fp);

	if (fm->meta_info.idx != 0) {
		status = false;
		goto error;
	}	

error:
	if (fp)
		fclose(fp);

	return status;
}

/* 
 * process_floppies
 *  	
 *
 * input: 
 * 	floppy_list: comma separated list of received floppies to verify, and write to
 * 	             final output file
 *
 * output:
 * 	bool: fail status if unable to open destination file
 * 	      or unable to process any floppy in list
 */
bool
process_floppies(char* floppy_list)
{
	char* tok = NULL;
	bool status = true;
	static FILE* fo = NULL;
	struct floppy_meta fm;

	status = process_meta_floppy(&fm);
	if (status == false)
		goto error;
	
	fo = fopen(OUTPUT_FILE_NAME, "w+");
	if (!fo) {
		error_print("Unable to open destination file '%s'\n", OUTPUT_FILE_NAME);
		status = false;
		goto error;
	}

	tok = strtok(floppy_list, ",");
	while (tok) {
		status = process_shard_floppy(&fm, tok, fo);
		if (status == false)
			goto error;
		tok = strtok(NULL, ",");
	}

error:
	if (fo) 
		fclose(fo);

	return status;
}
