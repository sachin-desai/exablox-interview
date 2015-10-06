#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include "recv.h"
#include "send.h"

static void
usage(void)
{
	printf("USAGE\n");
	printf("exablox <filename> ");
	printf("-s <input file> ");
	printf("-r <comma separated list of received floppies>\n");
	printf("      -s: File to shard and deliver as floppies\n");
	printf("      -r: List of floppies to merge into original file\n");
	printf("\n");

	exit(0);
}

static bool
parse_arguments(int argc, char** argv)
{
	int c;
	bool status = true;
	while((c = getopt(argc, argv, "s:r:h")) != -1) {
		switch(c) {
			case 's':
				status = create_floppies(optarg);
				if (status == false)
					return status;
				break;
			case 'r':
				status = process_floppies(optarg);
				if (status == false)
					return status;
				break;
			case 'h':
			default:
				usage();
		}

	}
	return true;	
}

int main(int argc, char** argv)
{
	return parse_arguments(argc, argv);
}
