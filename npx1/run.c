#include <stdio.h>
#include <stdlib.h>
#include "run.h"
#include "sys.h"

#define WORD_SIZE 4

/* bit sizes */
#define BIT_OC 6
#define BIT_REG 4
#define BIT_REG_MASK 0xf

/* instruction extracters */
#define IR_A ((ir >> BIT_OC) & BIT_REG_MASK)
#define IR_A2 (ir >> BIT_OC)
#define IR_B ((ir >> (BIT_OC + BIT_REG)) & BIT_REG_MASK)
#define IR_B2 ((ir >> (BIT_OC + BIT_REG)))
#define IR_C ((ir >> (BIT_OC + BIT_REG + BIT_REG)))

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

/* opcodes */

#define OC_SYS 0

#define OC_MOV 8
#define OC_MOVI (OC_MOV + 8)
#define OC_MOVI2 (OC_MOV + 16)
#define OC_CMP 9
#define OC_CMPI (OC_CMP + 8)
#define OC_CMPI2 (OC_CMP + 16)
#define OC_MUL 10
#define OC_MULI (OC_MUL + 8)
#define OC_MULI2 (OC_MUL + 16)
#define OC_DIV 11
#define OC_DIVI (OC_DIV + 8)
#define OC_DIVI2 (OC_DIV + 16)
#define OC_MOD 12
#define OC_MODI (OC_MOD + 8)
#define OC_MODI2 (OC_MOD + 16)
#define OC_ADD 13
#define OC_ADDI (OC_ADD + 8)
#define OC_ADDI2 (OC_ADD + 16)
#define OC_SUB 14
#define OC_SUBI (OC_SUB + 8)
#define OC_SUBI2 (OC_SUB + 16)

#define OC_LDB 32
#define OC_LDW 33
#define OC_STB 34
#define OC_STW 35

#define OC_POP 36
#define OC_PSH 37

#define OC_JB 48
#define OC_JF 49

#define OC_CB 50
#define OC_CF 51

#define OC_BFEQ 58
#define OC_BFNE 59
#define OC_BFLS 60
#define OC_BFGE 61
#define OC_BFLE 62
#define OC_BFGT 63

/*

memory-model

        ---------
        |       |
        |  dym  |      dynamic memory (heap)
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
	/* begin of dynamic memory */
	sys_dym = (int)(m + stm_size + prg_size);
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

void run_exec(char *fn, int mem_size, int stm_size)
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
