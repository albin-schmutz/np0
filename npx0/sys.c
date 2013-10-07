#include <stdio.h>
#include <stdlib.h>
#include "sys.h"

static FILE *f_in;
static FILE *f_out;

void sys_init(int argc, char *argv[])
{
	f_in = argc > 0 ? fopen(argv[0], "rb") : stdin;
	f_out = argc > 1 ? fopen(argv[1], "wb") : stdout;
}

void sys_call(int nr, int *result, int param)
{
	switch (nr) {
	case 0:
		exit(param);
		break;
	case -1:
		*result = fgetc(f_in);
		break;
	case -2:
		fputc(param, f_out);
		break;
	case -3:
		fprintf(f_out, "%i", param);
		break;
	default:
		fprintf(stderr, "sys call %i ???\n", nr);
		exit(1);
	}
}
