#ifndef MEM_H
#define MEM_H

#include "common.h"

uint32_t ram_load(uint8_t *mem, uint32_t addr, uint8_t size);

void ram_store(uint8_t *mem, uint32_t addr, uint32_t value, uint8_t size);

#endif
