#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asm.h"
#include "np.h"

#define MAX_LBLS 500

#define MB(a) (&m[(a)])
#define MW(a) ((int*)(&m[(a)]))
#define INC_PC (*pc) += WORD_SIZE

static char *mn[64] = {
	"SYS", "#1", "#2", "#3", "#4", "#5", "#6", "#7",
 	"MOV", "CMP", "MUL", "DIV", "MOD", "ADD", "SUB", "#15",
	"MOVI", "CMPI", "MULI", "DIVI", "MODI", "ADDI", "SUBI", "#23",
	"MOVI2", "CMPI2", "MULI2", "DIVI2", "MODI2", "ADDI2", "SUBI2", "#31",
	"LDB", "LDW", "STB", "STW", "POP", "PSH", "#38", "#39",
	"#40", "#41", "#42", "#43", "#44", "#45", "#46", "#47",
	"JB", "JF", "CB", "CF", "#52", "#53", "#54", "#55",
	"#56", "#57", "BFEQ", "BFNE", "BFLS", "BFGE", "BFLE", "BFGT"
};

static char *m;

static int read_char(FILE *f)
{
	return fgetc(f);
}

static int read_int(FILE *f)
{
	int i; fscanf(f, "%i", &i); return i;
}

static int read_oc(FILE *f)
{
	char buf[10]; fscanf(f, "%s", buf); int oc;
	for (oc = 0; oc < 64; ++oc) {
		if (!strcmp(mn[oc], buf)) return oc;
	}
	fprintf(stderr, "oc: %s: ???\n", buf);
	exit(1);
	return -1;
}

static int parse_eol(FILE *f)
{
	int c = read_char(f);
	while (c != -1 && c != '\n') { c = read_char(f); }
	return c == -1 ? c : read_char(f);
}

static int parse_oc(FILE *f, int *pc, int lbls[])
{
	int oc = read_oc(f); int i, j;
	switch (oc) {
	case OC_MOV: case OC_CMP:
	case OC_MUL: case OC_DIV: case OC_MOD:
	case OC_ADD: case OC_SUB:
	case OC_MOVI: case OC_CMPI:
	case OC_MULI: case OC_DIVI: case OC_MODI:
	case OC_ADDI: case OC_SUBI:
		j = (read_int(f) << BIT_OC);
		i = read_int(f);
		if (i >= -0xfffff && i < 0xfffff) {
			*MW(*pc) = oc | j | (i << (BIT_OC + BIT_REG)); INC_PC;
		} else {
			*MW(*pc) = (oc + 8) | j; INC_PC;
			*MW(*pc) = i; INC_PC;
		}
		break;
	case OC_SYS:
	case OC_LDB: case OC_LDW:
	case OC_STB: case OC_STW:
	case OC_POP: case OC_PSH:
		i = read_int(f);
		*MW(*pc) = oc | (i << BIT_OC) |
			(read_int(f) << (BIT_OC + BIT_REG));
		i = read_int(f);
		if (i >= -0x20000 && i < 0x20000) {
		  *MW(*pc) |= (i << (BIT_OC + BIT_REG + BIT_REG));
		} else {
			fprintf(stderr, "SYS2 und mem-2-ops not impl.\n");
			exit(1);
		}
		INC_PC;
		break;
	case OC_JB: case OC_CB:
		*MW(*pc) = oc | (((*pc - lbls[read_int(f)]) /
			WORD_SIZE + 1) << BIT_OC);
		INC_PC;
		break;
	case OC_JF: case OC_CF: case OC_BFEQ: case OC_BFNE:
	case OC_BFLS: case OC_BFGE: case OC_BFLE: case OC_BFGT:
		*MW(*pc) = oc; INC_PC;
		break;
	default:
		fprintf(stderr, "oc: %i: ???\n", oc);
		exit(1);
	}
	return parse_eol(f);
}

static int parse_file(FILE *f)
{
	int lbls[MAX_LBLS];
	int pc = 0; int line = 1; int i; int c = read_char(f);
	while (c != -1) {
		switch (c) {
		case '\n':
			c = read_char(f);
			line++;
			break;
		case '\r':
			c = read_char(f);
			break;
		case ':':
			lbls[read_int(f)] = pc;
			c = parse_eol(f);
			line++;
			break;
		case '=':
			i = read_int(f);
			*MW(lbls[i]) |=
				(((pc - lbls[i]) / WORD_SIZE - 1) << BIT_OC);
			c = parse_eol(f);
			line++;
			break;
		case '.':
			*MW(pc) = read_int(f); pc += WORD_SIZE;
			*MW(pc) = lbls[read_int(f)]; pc += WORD_SIZE;
			c = -1;
			break;
		case '\t':
			c = parse_oc(f, &pc, lbls);
			line++;
			break;
		default:
			fprintf(stderr, "%i: ???\n", line);
			exit(1);
		}
	}
	return pc;
}

void start_asm(char *fn_in, char *fn_out)
{
	m = malloc(MAX_MEM_SIZE);
	FILE *f_in = fopen(fn_in, "rb");
	if (f_in) {
		FILE *f_out = fopen(fn_out, "wb");
		if (f_out) {
			int size = parse_file(f_in);
			fclose(f_in);
			if (size > 0) {
				fwrite(m, 1, size, f_out);
				fclose(f_out);
			} else {
				exit(1);
			}
		} else {
			perror(fn_out);
			exit(1);
		}
	} else {
		perror(fn_in);
		exit(1);
	}
}

void start_dis(char *fn)
{
	m = malloc(MAX_MEM_SIZE);
	FILE *f = fopen(fn, "rb");
	if (f) {
		int size = fread(m, 1, MAX_MEM_SIZE, f);
		fclose(f);
		int pc = 0; size -= (EPILOG_SIZE * WORD_SIZE);
		while (pc < size) {
			int ir = *MW(pc);
			int oc = ir & 0x3f;
			printf("%08x: %s ", pc, mn[oc]);
			pc += WORD_SIZE;
			switch (oc) {
			case OC_MOV: case OC_CMP:
			case OC_MUL: case OC_DIV: case OC_MOD:
			case OC_ADD: case OC_SUB:
			case OC_MOVI: case OC_CMPI:
			case OC_MULI: case OC_DIVI: case OC_MODI:
			case OC_ADDI: case OC_SUBI:
				printf("%i %i\n", IR_A, IR_B2);
				break;
			case OC_MOVI2: case OC_CMPI2:
			case OC_MULI2: case OC_DIVI2: case OC_MODI2:
			case OC_ADDI2: case OC_SUBI2:
				printf("%i %i\n", IR_A, *MW(pc));
				pc += WORD_SIZE;
				break;
			case OC_SYS:
			case OC_LDB: case OC_LDW:
			case OC_STB: case OC_STW:
			case OC_POP: case OC_PSH:
				printf("%i %i %i\n", IR_A, IR_B, IR_C);
				break;
			case OC_JB: case OC_CB:
				printf("%08x\n", pc - IR_A2 * WORD_SIZE);
				break;
			case OC_JF: case OC_CF:
			case OC_BFEQ: case OC_BFNE: case OC_BFLS:
			case OC_BFGE: case OC_BFLE: case OC_BFGT:
				printf("%08x\n", pc + IR_A2 * WORD_SIZE);
				break;
			default:
				fprintf(stderr, "pc: %08x, oc: %i: ???\n",
					pc - WORD_SIZE, oc);
				exit(1);
			}
		} 
	} else {
		perror(fn);
		exit(1);
	}
}
