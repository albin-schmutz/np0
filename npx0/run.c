#include <stdio.h>
#include <stdlib.h>
#include "run.h"
#include "sys.h"
#include "np.h"

#define NBR_REGS 16

/* special registers */
#define GP r[11]
#define SP r[13]
#define RT r[14]
#define PC r[15]

/* increments and jumps */
#define INC_PC PC += WORD_SIZE
#define JMP_F PC += IR_A2 * WORD_SIZE
#define JMP_B PC -= IR_A2 * WORD_SIZE

/*

memory-model

        ---------
        |       |
        |  dym  |      dynamic memory
        |       |
        ---------
        |       |  <-- program epilog (var-size, entry-point)
        |       |
PC -->  |  prg  |      program memory
        |       |
GP -->  ---------
        |       |      static memory
SP -->  |  stm  |      vars & stack
        |       |
m -->   ---------

*/

static void run(char *m, int stm_size, int prg_size)
{
	int r[NBR_REGS];
	int z, n;
	GP = (int)(m + stm_size);
	/* var-size */
	SP = *((int*)&m[stm_size + prg_size - 2 * WORD_SIZE]);
	SP += GP;
	/* push begin of dym */
	SP -= WORD_SIZE; *((int*)SP) = (int)(m + stm_size + prg_size);
	/* push return address 0 */
	SP -= WORD_SIZE; *((int*)SP) = 0;
	/* entry-point */
	PC = *((int*)&m[stm_size + prg_size - 1 * WORD_SIZE]);
	PC += GP;
	while (PC) {
		int i; int ir = *((int*)PC); INC_PC;
		switch (ir & 0x3f) {
		case OC_SYS:
			sys_call(IR_C, &r[IR_A], r[IR_B]);
			break;
		case OC_MOV:
			r[IR_A] = r[IR_B];
			break;
		case OC_MOVI:
			r[IR_A] = IR_B2;
			break;
		case OC_MOVI2:
			i = *((int*)PC); INC_PC;
			r[IR_A] = i;
			break;
		case OC_CMP:
			z = (r[IR_A] == r[IR_B]);
			n = (r[IR_A] < r[IR_B]);
			break;
		case OC_CMPI:
			z = (r[IR_A] == IR_B2);
			n = (r[IR_A] < IR_B2);
			break;
		case OC_CMPI2:
			i = *((int*)PC); INC_PC;
			z = (r[IR_A] == i);
			n = (r[IR_A] < i);
			break;
		case OC_MUL:
			r[IR_A] *= r[IR_B];
			break;
		case OC_MULI:
			r[IR_A] *= IR_B2;
			break;
		case OC_MULI2:
			i = *((int*)PC); INC_PC;
			r[IR_A] *= i;
			break;
		case OC_DIV:
			r[IR_A] /= r[IR_B];
			break;
		case OC_DIVI:
			r[IR_A] /= IR_B2;
			break;
		case OC_DIVI2:
			i = *((int*)PC); INC_PC;
			r[IR_A] /= i;
			break;
		case OC_MOD:
			r[IR_A] %= r[IR_B];
			break;
		case OC_MODI:
			r[IR_A] %= IR_B2;
			break;
		case OC_MODI2:
			i = *((int*)PC); INC_PC;
			r[IR_A] %= i;
			break;
		case OC_ADD:
			r[IR_A] += r[IR_B];
			break;
		case OC_ADDI:
			r[IR_A] += IR_B2;
			break;
		case OC_ADDI2:
			i = *((int*)PC); INC_PC;
			r[IR_A] += i;
			break;
		case OC_SUB:
			r[IR_A] -= r[IR_B];
			break;
		case OC_SUBI:
			r[IR_A] -= IR_B2;
			break;
		case OC_SUBI2:
			i = *((int*)PC); INC_PC;
			r[IR_A] -= i;
			break;
		case OC_LDB:
			r[IR_A] = *((char*)(r[IR_B] + IR_C));
			break;
		case OC_LDW:
			r[IR_A] = *((int*)(r[IR_B] + IR_C));
			break;
		case OC_STB:
			*((char*)(r[IR_B] + IR_C)) = r[IR_A];
			break;
		case OC_STW:
			*((int*)(r[IR_B] + IR_C)) = r[IR_A];
			break;
		case OC_POP:
			i = IR_B;
			r[IR_A] = *((int*)r[i]);
			r[i] += IR_C;
			break;
		case OC_PSH:
			i = IR_B;
			r[i] += IR_C;
			*((int*)r[i]) = r[IR_A];
			break;
		case OC_JF:
			JMP_F;
			break;
		case OC_CF:
			RT = PC; JMP_F;
			break;
		case OC_JB:
			JMP_B;
			break;
		case OC_CB:
			RT = PC; JMP_B;
			break;
		case OC_BFEQ:
			if (z) JMP_F;
			break;
		case OC_BFNE:
			if (!z) JMP_F;
			break;
		case OC_BFLS:
			if (n) JMP_F;
			break;
		case OC_BFGE:
			if (!n) JMP_F;
			break;
		case OC_BFLE:
			if (z || n) JMP_F;
			break;
		case OC_BFGT:
			if (!z && !n) JMP_F;
			break;
		default:
			fprintf(stderr, "run oc: %i ???\n", ir & 0x3f);
			exit(1);
		}
	}
}

void execute(char *fn, int mem_size, int stm_size)
{
	FILE *f = fopen(fn, "rb");
	if (f) {
		char *m = malloc(mem_size);
		int prg_size = fread(m + stm_size, 1, mem_size - stm_size, f);
		fclose(f);
		run(m, stm_size, prg_size);
	} else {
		perror(NULL);
		exit(1);
	}
}
