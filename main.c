#include <stdio.h>

#define MOV 0
#define ADD 1
#define SUB 2
#define AND 3
#define OR  4
#define SL  5
#define SR  6
#define SRA 7
#define LDL 8
#define LDH 9
#define CMP 10
#define JE  11
#define JMP 12
#define LD  13
#define ST  14
#define HLT 15

#define REG0 0
#define REG1 1
#define REG2 2
#define REG3 3
#define REG4 4
#define REG5 5
#define REG6 6
#define REG7 7

short reg[8];
short rom[256];
short ram[256];

void assembler(void);

short mov(short, short);
short add(short, short);
short sub(short, short);
short and(short, short);
short or(short, short);
short sl(short);
short sr(short);
short sra(short);
short ldl(short, short);
short ldh(short, short);
short cmp(short, short);
short je(short);
short jmp(short);
short ld(short, short);
short st(short, short);
short hlt(void);

short op_code(short);
short op_regA(short);
short op_regB(short);
short op_data(short);
short op_addr(short);

int main(void) {
  short pc;
  short ir;
  short flag_eq;

  assembler();

  pc = 0;
  flag_eq = 0;

  do {
    ir = rom[pc];
    printf("%5d %5x %5d %5d %5d %5d\n", pc, ir, reg[0], reg[1], reg[2], reg[3]);

    pc = pc + 1;

    switch (op_code(ir)) {
      case MOV:
        reg[op_regA(ir)] = reg[op_regB(ir)];
        break;
      case ADD:
        reg[op_regA(ir)] = reg[op_regA(ir)] + reg[op_regB(ir)];
        break;
      case SUB:
        reg[op_regA(ir)] = reg[op_regA(ir)] - reg[op_regB(ir)];
        break;
      case OR:
        reg[op_regA(ir)] = reg[op_regA(ir)] | reg[op_regB(ir)];
        break;
      case AND:
        reg[op_regA(ir)] = reg[op_regA(ir)] & reg[op_regB(ir)];
        break;
      case SL:
        reg[op_regA(ir)] = reg[op_regA(ir)] << 1;
        break;
      case SR:
        reg[op_regA(ir)] = reg[op_regA(ir)] >> 1;
        break;
      case SRA:
        reg[op_regA(ir)] = (reg[op_regA(ir)] & 0x8000) | (reg[op_regA(ir)] >> 1);
        break;
      case LDL:
        reg[op_regA(ir)] = (reg[op_regA(ir)] & 0xff00) | (op_data(ir) & 0x00ff);
        break;
      case LDH:
        reg[op_regA(ir)] = (op_data(ir) << 8) | (reg[op_regA(ir)]& 0x00ff);
        break;
      case CMP:
        if (reg[op_regA(ir)] == reg[op_regB(ir)]) {
          flag_eq = 1;
        } else {
          flag_eq = 0;
        }
        break;
      case JE:
        if (flag_eq == 1) pc = op_addr(ir);
        break;
      case JMP:
        pc = op_addr(ir);
        break;
      case LD:
        reg[op_regA(ir)] = ram[op_addr(ir)];
        break;
      case ST:
        ram[op_addr(ir)] = reg[op_regA(ir)];
        break;
      default:
        break;
    }
  } while(op_code(ir) != HLT);

  printf("ram[64] = %d\n", ram[64]);

  return 0;
}

void assembler(void) {
  rom[0] = ldh(REG0, 0);
  rom[1] = ldl(REG0, 0);
  rom[2] = ldh(REG1, 0);
  rom[3] = ldl(REG1, 1);
  rom[4] = ldh(REG2, 0);
  rom[5] = ldl(REG2, 0);
  rom[6] = ldh(REG3, 0);
  rom[7] = ldl(REG3, 10);
  rom[8] = add(REG2, REG1);
  rom[9] = add(REG0, REG2);
  rom[10] = st(REG0, 64);
  rom[11] = cmp(REG2, REG3);
  rom[12] = je(14);
  rom[13] = jmp(8);
  rom[14] = hlt();
}

short mov(short ra, short rb) {
  return ((MOV << 11) | (ra << 8) | (rb << 5));
}
short add(short ra, short rb) {
  return ((ADD << 11) | (ra << 8) | (rb << 5));
}
short sub(short ra, short rb) {
  return ((SUB << 11) | (ra << 8) | (rb << 5));
}
short and(short ra, short rb) {
  return ((AND << 11) | (ra << 8) | (rb << 5));
}
short or(short ra, short rb) {
  return ((OR << 11) | (ra << 8) | (rb << 5));
}
short sl(short ra) {
  return ((SL << 11) | (ra << 8));
}
short sr(short ra) {
  return ((SR << 11) | (ra << 8));
}
short sra(short ra) {
  return ((SRA << 11) | (ra << 8));
}
short ldl(short ra, short ival) {
  return ((LDL << 11) | (ra << 8) | (ival & 0x00ff));
}
short ldh(short ra, short ival) {
  return ((LDH << 11) | (ra << 8) | (ival & 0x00ff));
}
short cmp(short ra, short rb) {
  return ((CMP << 11) | (ra << 8) | (rb << 5));
}
short je(short addr) {
  return ((JE << 11) | (addr & 0x00ff));
}
short jmp(short addr) {
  return ((JMP << 11) | (addr & 0x00ff));
}
short ld(short ra, short addr) {
  return ((LD << 11) | (ra << 8) | (addr & 0x00ff));
}
short st(short ra, short addr) {
  return ((ST << 11) | (ra << 8) | (addr & 0x00ff));
}
short hlt(void) {
  return (HLT << 11);
}

short op_code(short ir) {
  return (ir >> 11);
}
short op_regA(short ir) {
  return ((ir >> 8) & 0x0007);
}
short op_regB(short ir) {
  return ((ir >> 5) & 0x0007);
}
short op_data(short ir) {
  return (ir & 0x00ff);
}
short op_addr(short ir) {
  return (ir & 0x00ff);
}
