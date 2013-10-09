#include <stdio.h>
#include <stdlib.h>
#include "sys.h"

int sys_dym;

static int args_count;
static char **args;

struct STACK_EXIT {
	int nr;
};

struct STACK_GET_INFO {
	int ix;
};

struct STACK_GET_ARG {
	int ix;
};

struct STACK_OPEN_FILE {
	int mode;
	char *name;
};

struct STACK_CLOSE_FILE {
	FILE *file;
};

struct STACK_FILE_CHANNEL {
	int len;
	char *addr;
	FILE *file;
};

static void get_info(int *res, struct STACK_GET_INFO *gi)
{
	switch (gi->ix) {
	case 1:
		*res = sys_dym;
		break;
	case 2:
		*res = args_count;
		break;
	case 3:
		*res = (int)stdin;
		break;
	case 4:
		*res = (int)stdout;
		break;
	case 5:
		*res = (int)stderr;
		break;
	default:
		fprintf(stderr, "get info %i ???\n", gi->ix);
		exit(1);
	}
}

static void open_file(int *res, struct STACK_OPEN_FILE *of)
{
	switch (of->mode) {
	case 1:
		*res = (int)fopen(of->name, "rb");
		break;
	case 2:
		*res = (int)fopen(of->name, "wb");
		break;
	default:
		fprintf(stderr, "open file mode %i ???\n", of->mode);
		exit(1);
	}
}

static void read_file(int *res, struct STACK_FILE_CHANNEL *fc)
{
	*res = fread(fc->addr, 1, fc->len, fc->file);
}

static void write_file(int *res, struct STACK_FILE_CHANNEL *fc)
{
	*res = fwrite(fc->addr, 1, fc->len, fc->file);
}

void sys_init(int argc, char *argv[])
{
	args_count = argc;
	args = argv;
}

void sys_call(int nr, int *res, int par)
{
	switch (nr) {
	case 0:
		exit(((struct STACK_EXIT*)par)->nr);
		break;
	case 1:
		get_info(res, (struct STACK_GET_INFO*)par);
		break;
	case 2:
		*res = (int)args[((struct STACK_GET_ARG*)par)->ix];
		break;
	case 3:
		open_file(res, (struct STACK_OPEN_FILE*)par);
		break;
	case 4:
		fclose(((struct STACK_CLOSE_FILE*)par)->file);
		break;
	case 5:
		read_file(res, (struct STACK_FILE_CHANNEL*)par);
		break;
	case 6:
		write_file(res, (struct STACK_FILE_CHANNEL*)par);
		break;
	case -3:
		fprintf(stderr, "%08x\n", (int)par);
		break;
	case -4:
		fprintf(stderr, "%s\n", (char*)par);
		break;
	default:
		fprintf(stderr, "sys call nr %i ???\n", nr);
		exit(1);
	}
}
