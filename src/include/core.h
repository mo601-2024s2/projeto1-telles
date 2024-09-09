#ifndef CORE_H
#define CORE_H

#include "common.h"
#include "mem.h"
#include "ringbuffer.h"

/* ref: https://en.wikichip.org/wiki/risc-v/registers*/
typedef struct {
  uint32_t regs[32];
  size_t pc;
  uint8_t *ram;
} CORE;

/* Instruction Format */
/* ref: https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf pag 11*/
typedef struct {
  uint8_t opcode;
  uint8_t rd;
  uint8_t rs1;
  uint8_t rs2;
  uint8_t funct3;
  uint8_t funct7;
} INST;

/* Log Struct */
typedef struct {
  uint32_t h_pc;		// Ex: PC=00000100
  uint32_t h_inst;  	// [012345678]
  uint32_t h_rd;   		// indicado pelos bits 7-11, registrador de destino (rd) , eg r15  x15=000AAA00, após instrução
  uint32_t rd;
  uint32_t h_rs1;		// registrador de origem 1 (rs1), eg r3  x03=99988877, registrador indicado pelos bits 15-19, antes da instrução
  uint32_t rs1;
  uint32_t h_rs2;   	//  registrador de origem 2 (rs2), idem utilizando os bits 20-24, antes da instrução
  uint32_t rs2;
  char mne[64]; 		//  mnemônico
} RLOG;

uint32_t core_load(CORE *core, uint32_t addr, uint8_t size);
void core_store(CORE *cup, uint32_t addr, uint32_t value, uint8_t size);
void core_decode(uint32_t raw_inst, INST *inst);
void core_execute(CORE *, uint32_t inst, RLOG *);

#endif
