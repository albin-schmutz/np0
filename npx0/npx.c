#include <stdio.h>
#include <string.h>
#include "run.h"
#include "sys.h"

static void print_usage(void)
{
	fprintf(stderr, "\nusage: npx <bytecode-file>\n\n");
}

int main(int argc, char *argv[])
{
	if (argc >= 2 ) {
		sys_init(argc - 2, argc > 2 ? &argv[2] : NULL);
		execute(argv[1], 0x200000, 0x100000);
	} else {
		print_usage();
	}
	return 0;
}
