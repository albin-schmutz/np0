#include <stdio.h>
#include <string.h>
#include "asm.h"

static void print_usage(void)
{
	fprintf(stderr, "\nusage: npa <command> { <param> }\n\n\n");
	fprintf(stderr, "commands:\n\n\n");
	fprintf(stderr, "asm <np-assembler-file> <bytecode-file>\n\n");
	fprintf(stderr, "\tcompile np-assembler to bytecode\n\n\n");
	fprintf(stderr, "dis <bytecode-file>\n\n");
	fprintf(stderr, "\tdisassemble bytecode file\n\n\n");
	fprintf(stderr, "help\n\n");
	fprintf(stderr, "\tshow this help\n\n\n");
}

int main(int argc, char *argv[])
{
	if (argc >= 2 ) {
		char *cmd = argv[1];
		if (!strcmp(cmd, "asm")) {
			if (argc >= 4 ) {
				start_asm(argv[2], argv[3]);
			} else {
				print_usage();
			}
		} else if (!strcmp(cmd, "dis")) {
			if (argc >= 3 ) {
				start_dis(argv[2]);
			} else {
				print_usage();
			}
		} else {
			print_usage();
		}
	} else {
		print_usage();
	}
	return 0;
}
