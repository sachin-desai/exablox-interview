#ifndef __OPTIONS_H__
#define __OPTIONS_H__

typedef char OPTION;
typedef char* OPTION_FILE;

typedef struct usr_args {
	int arg_count;
	int floppy_count;

	char** arg_list;
	char** floppy_list;

	OPTION send;	
	OPTION recv;
	OPTION help;
	OPTION meta;

	OPTION_FILE src_file;
	OPTION_FILE dst_file;
	OPTION_FILE meta_file;
}usr_args;

void init_args(int, char**, usr_args*);
int parse_args(usr_args*);
void free_args(usr_args*);
void usage(void);

#endif
