#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usr_args.h"
#include "recv.h"
#include "send.h"

int main(int argc, char** argv)
{
	int ret = 0;
	struct usr_args args;

	init_args(argc, argv, &args);

	if (parse_args(&args)) {
		free_args(&args);
		usage(); /* exits */
	}

	if (args.send)
		ret = create_floppies(&args);

	if (args.recv)
		ret = process_floppies(&args);

	free_args(&args);

	return ret;
}
