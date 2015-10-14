#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "usr_args.h"

void 
init_args(int argc, char** argv, usr_args* opt)
{
	opt->send = opt->recv = opt->help = opt->meta = 0;

	opt->src_file = NULL;
	opt->dst_file = NULL;
	opt->meta_file = NULL;

	opt->arg_list = argv;
	opt->arg_count = argc;
}

void
free_args(usr_args* opt)
{
	if (opt->src_file)
		free(opt->src_file);

	if (opt->dst_file)
		free(opt->dst_file);

	if (opt->meta_file)
		free(opt->meta_file);
}

void
usage(void)
{
	printf("USAGE\n");
	printf("exablox -m <meta file> [-s <src_file> | -r <dst_file> <floppy list>]");
	printf("<OPTIONS>\n");
	printf("      -m <meta file>: Meta data filename, required for both receiver and sender\n");
	printf("      -s <src file>:  File to shard and deliver as floppies\n");
	printf("      -r <dst file>:  File to reconstruct floppy data into\n");
	printf("         <floppy list> Floppy files in format 'floppy.NUM'\n");
	printf("\n");
}

int
parse_args(usr_args* args)
{
	int c;
	int ret = 0; 

	while((c = getopt(args->arg_count, args->arg_list, "s:r:m:h")) != -1) {
		switch(c) {
			case 'm':
				args->meta = 1;
				args->meta_file = strdup(optarg);
				break;
			case 's':
				args->send = 1;
				args->src_file = strdup(optarg);
				break;
			case 'r':
				args->recv = 1;
				args->dst_file = strdup(optarg);
				break;
			case 'h':
				args->help = 1;
				break;
			default:
				break;		
		}

	}

	/* 
	 * 1. user must choose either send, or recv 
	 * 2. required meta flag
	 * 3. help option 
	 * 4. insufficient arguments
	 */
	if ((args->send && args->recv) || args->arg_count < 2 ||
		!(args->meta) || args->help) {
		ret = 1;
		goto error;
	}

	/* floppy list, retrieved without a flag */
	args->floppy_list = args->arg_list + optind;
	args->floppy_count = args->arg_count - optind;
	
error:
	return ret;
}
