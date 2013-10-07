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
