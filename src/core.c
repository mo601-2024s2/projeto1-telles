#include "include/core.h"

INST inst;

uint32_t core_load(CORE *core, uint32_t addr, uint8_t size) {
  return ram_load(core->ram, addr, size);
}
void core_store(CORE *core, uint32_t addr, uint32_t value, uint8_t size) {
  ram_store(core->ram, addr, value, size);
}

/* ref: https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf*/
void core_decode(uint32_t inst_raw, INST *inst) {
  inst->opcode = inst_raw & 0x7F;
  inst->rd = (inst_raw >> 7) & 0x1F;
  inst->rs1 = (inst_raw >> 15) & 0x1F;
  inst->rs2 = (inst_raw >> 20) & 0x1F;
  inst->funct3 = (inst_raw >> 12) & 0x7;
  inst->funct7 = (inst_raw >> 25) & 0x7F;
}

/* Immediate Encoding Variants */
/* pag 12 https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf*/
static inline int32_t i_imm(uint32_t inst_raw) {
  return ((int32_t)inst_raw) >> 20;
}
static inline int32_t s_imm(uint32_t inst_raw) {
  return ((int32_t)(inst_raw & 0xFE000000)) >> 20 |
         ((int32_t)(inst_raw >> 7) & 0x1F);
}
static inline int32_t u_imm(uint32_t inst_raw) {
  return ((int32_t)inst_raw & 0xFFFFF000);
}
static inline int32_t j_imm(uint32_t inst_raw) {
  return ((uint32_t)(((int32_t)(inst_raw & 0x80000000)) >> 11)) |
         (inst_raw & 0xFF000) |
         ((inst_raw >> 9) & 0x800) |
         ((inst_raw >> 20) & 0x7FE);
}
static inline int32_t b_imm(uint32_t inst_raw) {
  return ((uint32_t)(((int32_t)(inst_raw & 0x80000000)) >> 19)) |
         ((inst_raw & 0x80) << 4) | ((inst_raw >> 20) & 0x7E0) |
         ((inst_raw >> 7) & 0x1E);
}
/* */

void core_execute(CORE *core, uint32_t inst_raw, RLOG *log) {
  core_decode(inst_raw, &inst);
  log->rs1 = inst.rs1;
  log->h_rs1 = core->regs[inst.rs1];
  log->rs2 = inst.rs2;
  log->h_rs2 = core->regs[inst.rs2];
  core->regs[0] = 0;
  
  switch (inst.opcode) {
		  
  // LOAD
  case 0x3: {
	/* pag 19 https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf*/
    int32_t imm = i_imm(inst_raw);
    uint32_t addr = core->regs[inst.rs1] + imm;
    uint32_t val = 0;
    switch (inst.funct3) {
    // BYTE
		case 0x0: {
      val = (int8_t)core_load(core, addr, 8);
      core->regs[inst.rd] = val;
    } break;
	// HALF WORD
    case 0x1: {
      val = (int16_t)core_load(core, addr, 16);
      core->regs[inst.rd] = val;
    } break;
	// WORD
    case 0x2: {
      val = (int32_t)core_load(core, addr, 32);
      core->regs[inst.rd] = val;
    } break;
	// BYTE UNSIGNED
    case 0x4: {
      val = core_load(core, addr, 8);
      core->regs[inst.rd] = val;
    } break;
	// HALF WORD UNSIGNED
    case 0x5: {
      val = core_load(core, addr, 16);
      core->regs[inst.rd] = val;
    } break;
	// WORD UNSIGNED
    case 0x6: {
      val = core_load(core, addr, 32);
      core->regs[inst.rd] = val;
    } break;
    default: ;
    }
	//write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "LOAD____dest=%02d_width=%02d_base=%02d_offset=%04d",inst.rd, inst.funct3, inst.rs1, imm);
  } break;
  
  //STORE
  case 0x23: {
	/* pag 19 https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf*/
    int32_t imm = s_imm(inst_raw);
    uint32_t addr = core->regs[inst.rs1] + imm;
    uint32_t val = core->regs[inst.rs2];
    switch (inst.funct3) {
	// BYTE
    case 0x0: core_store(core, addr, val, 8); break;
	// HALF WORD
    case 0x1: core_store(core, addr, val, 16); break;
	// WORD
    case 0x2: core_store(core, addr, val, 32); break;
    default: ;
    }
    //write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "STORE___width=%02d_base=%02d_src=%02d_offset=%04d", inst.funct3, inst.rs1, inst.rs2, imm);
  } break;
  
  //I-type Integer computation
  // ADDI, ANDI, ORI, XORI
  case 0x13: {
    int32_t imm = i_imm(inst_raw);
	char func3[5] = "";
    switch (inst.funct3) {
	// ADDI
    case 0x0: {
      core->regs[inst.rd] = core->regs[inst.rs1] + imm;
	  strncpy(func3, "ADDI", 5);
    } break;
	// XORI
    case 0x4: {
      core->regs[inst.rd] = core->regs[inst.rs1] ^ imm;
	  strncpy(func3, "XORI", 5);
    } break;
	//ORI
    case 0x6: {
      core->regs[inst.rd] = core->regs[inst.rs1] | imm;
	  strncpy(func3, "ORI", 5);
    } break;
	// ANDI
    case 0x7: {
      core->regs[inst.rd] = core->regs[inst.rs1] & imm;
	  strncpy(func3, "ANDI", 5);
    } break;
    default: ;
    }
	//write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "OP-IMM__dest=%02d_func=%s_src=%02d_I-imm=%04d", inst.rd, &func3[0], inst.rs2, imm);
  } break;

  // The R-type Integer computation
  case 0x33: {
	char func37[5] = "";
	// ADD
    if (inst.funct3 == 0x0 && inst.funct7 == 0x0) {
      core->regs[inst.rd] = core->regs[inst.rs1] + core->regs[inst.rs2];
	  strncpy(func37, "ADD", 5);
	// MUL
    } else if (inst.funct3 == 0x0 && inst.funct7 == 0x1) {
      core->regs[inst.rd] = core->regs[inst.rs1] * core->regs[inst.rs2];
	  strncpy(func37, "MUL", 5);
	// SUB
    } else if (inst.funct3 == 0x0 && inst.funct7 == 0x20) {
      core->regs[inst.rd] = core->regs[inst.rs1] - core->regs[inst.rs2];
	  strncpy(func37, "SUB", 5);
	// XOR
    } else if (inst.funct3 == 0x4 && inst.funct7 == 0x0) {
      core->regs[inst.rd] = core->regs[inst.rs1] ^ core->regs[inst.rs2];
	  strncpy(func37, "XOR", 5);
	// OR
    } else if (inst.funct3 == 0x6 && inst.funct7 == 0x0) {
      core->regs[inst.rd] = core->regs[inst.rs1] | core->regs[inst.rs2];
	  strncpy(func37, "OR", 5);
	// AND
    } else if (inst.funct3 == 0x7 && inst.funct7 == 0x0) {
      core->regs[inst.rd] = core->regs[inst.rs1] & core->regs[inst.rs2];
	  strncpy(func37, "AND", 5);
    }
	//write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "OP______dest=%02d_func=%s_src1=%02d_src2=%02d", inst.rd, &func37[0], inst.rs1, inst.rs2);
  } break;

  // LUI
  case 0x37: {
    int32_t imm = u_imm(inst_raw);
    core->regs[inst.rd] = imm;
	//write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "LUI_____dest=%02d_U-imm=%07d", inst.rd, imm);
  } break;

  // AUIPC
  case 0x17: {
    int32_t imm = u_imm(inst_raw);
    core->regs[inst.rd] = (core->pc - 4) + imm;
	//write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "AUIPC___dest=%02d_U-imm=%07d", inst.rd, imm);
  } break;

  // JAL
  case 0x6F: {
    int32_t imm = j_imm(inst_raw);
    core->regs[inst.rd] = core->pc;
    int32_t jmp_addr = imm + (core->pc - 4);
    core->pc = jmp_addr;
	//write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "JAL_____dest=%02d_offset=%07d", inst.rd, imm);
  } break;

  // JALR
  case 0x67: {
    core->regs[inst.rd] = core->pc;
    int32_t imm = i_imm(inst_raw);
    int32_t jmp_addr = core->regs[inst.rs1] + imm;
    core->pc = jmp_addr;
	//write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "JALR____dest=%02d_base=%02d_offset=%07d", inst.rd, inst.rs1, imm);
  } break;

  // BRANCH Conditional branches: BEQ, BNE, BLT[U], BGE[U]
  case 0x63: {
    int32_t imm = b_imm(inst_raw);
	char func3[5] = "";
    switch (inst.funct3) {
	// BEQ
    case 0x0: {
      int c = core->regs[inst.rs1] == core->regs[inst.rs2];
      if (c) core->pc += imm - 4;
	  strncpy(func3, "BEQ", 5);
    } break;
	// BNE
    case 0x1: {
      int c = core->regs[inst.rs1] != core->regs[inst.rs2];
      if (c) core->pc += imm - 4;
	  strncpy(func3, "BNE", 5);
    } break;
	// BLT
    case 0x4: {
      int c = ((int32_t)core->regs[inst.rs1]) < ((int32_t)core->regs[inst.rs2]);
      if (c) core->pc += imm - 4;
      strncpy(func3, "BLT", 5);
    } break;
	// BGE
    case 0x5: {
      int c = ((int32_t)core->regs[inst.rs1]) >= ((int32_t)core->regs[inst.rs2]);
      if (c) core->pc += imm - 4;
      strncpy(func3, "BGE", 5);
    } break;
	// BLTU
    case 0x6: {
      int c = core->regs[inst.rs1] < core->regs[inst.rs2];
      if (c) core->pc += imm - 4;
      strncpy(func3, "BLTU", 5);
    } break;
	// BGEU
    case 0x7: {
      int c = core->regs[inst.rs1] >= core->regs[inst.rs2];
      if (c) core->pc += imm - 4;
      strncpy(func3, "BGEU", 5);
    } break;
    default: ;
    }
	//write mne description on log struct
    snprintf(log->mne, sizeof(log->mne), "BRANCH__func=%s_src1=%02d_src2=%02d_offset=%07d", &func3[0], inst.rs1, inst.rs2, imm);
  } break;
  default: ;
  }

  log->rd = inst.rd;
  log->h_rd = core->regs[inst.rd];
}