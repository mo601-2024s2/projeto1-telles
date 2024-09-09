#include "include/mem.h"
#include "include/common.h"

static uint32_t ram_load8(uint8_t *, uint32_t);
static uint32_t ram_load16(uint8_t *, uint32_t);
static uint32_t ram_load32(uint8_t *, uint32_t);
static void ram_store8(uint8_t *, uint32_t, uint32_t);
static void ram_store16(uint8_t *, uint32_t, uint32_t);
static void ram_store32(uint8_t *, uint32_t, uint32_t);

uint32_t ram_load(uint8_t *mem, uint32_t addr, uint8_t size) {
  switch (size) {
  case 8:  return ram_load8(mem, addr);
  case 16: return ram_load16(mem, addr);
  case 32: return ram_load32(mem, addr);
  default: ;
  }
  return 0;
}

void ram_store(uint8_t *mem, uint32_t addr, uint32_t value, uint8_t size) {
  switch (size) {
  case 8:  ram_store8(mem, addr, value);  break;
  case 16: ram_store16(mem, addr, value); break;
  case 32: ram_store32(mem, addr, value); break;
  default: ;
  }
}

static uint32_t ram_load8(uint8_t *mem, uint32_t addr) {
  return (uint32_t)mem[addr];
}

static uint32_t ram_load16(uint8_t *mem, uint32_t addr) {
  return ((uint32_t)mem[addr] |
		  ((uint32_t)mem[addr + 1] << 8));
}

static uint32_t ram_load32(uint8_t *mem, uint32_t addr) {
  return ((uint32_t)mem[addr] |
		  ((uint32_t)mem[addr + 1] << 8) |
          ((uint32_t)mem[addr + 2] << 16) |
          ((uint32_t)mem[addr + 3] << 24));
}

static void ram_store8(uint8_t *mem, uint32_t addr, uint32_t value) {
  mem[addr] = (uint8_t)value;
}

static void ram_store16(uint8_t *mem, uint32_t addr, uint32_t value) {
  mem[addr] = (uint8_t)value & 0xFF;
  mem[addr + 1] = (uint8_t)(value >> 8) & 0xFF;
}

static void ram_store32(uint8_t *mem, uint32_t addr, uint32_t value) {
  mem[addr] = (uint8_t)value & 0xFF;
  mem[addr + 1] = (uint8_t)(value >> 8) & 0xFF;
  mem[addr + 2] = (uint8_t)(value >> 16) & 0xFF;
  mem[addr + 3] = (uint8_t)(value >> 24) & 0xFF;
}
