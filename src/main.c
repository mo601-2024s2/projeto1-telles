//#include <ansi_c.h>
#include "include/common.h"
#include "include/core.h"
#include "include/mem.h"
#include "include/ringbuffer.h"

int main(int argc, char *argv[]) {

  /* Check if there is code path arg */
  if (argc < 2) {
    printf("Requires rv32im binary [filename]\n");
    exit(-1);
  }

  /* Upload instruction list from binary file to the code instruction_vector array*/
  const char *filename = argv[1];
  FILE *binfile = fopen(filename, "rb"); // read binary mode
  if (!binfile) {
    printf("FAIL to open the file.\n");
    exit(-1);
  }
  fseek(binfile, 0L, SEEK_END); // move pointer to the end
  size_t inst_vector_length = ftell(binfile); // get pointer position
  rewind(binfile); // return pointer to the begining
  uint8_t *inst_vector = (uint8_t *)malloc(inst_vector_length); // allocate instructions vector in the binary file lenght in bytes
  fread(inst_vector, sizeof(uint8_t), inst_vector_length, binfile); //copy binary file to instructions vector
  fclose(binfile);

  /* Allocate CORE struct and its resources*/
  /* reference: https://en.wikichip.org/wiki/risc-v/registers*/
  CORE *core = (CORE *)malloc(sizeof(CORE));
  memset(core->regs, 0, sizeof(core->regs));
  core->regs[0] = 0;
  core->regs[2] = 8192000; // 8MB, number candidate for #define
  core->pc = 0x0;
  core->ram = (uint8_t *)malloc(8192000);
  memcpy(core->ram, inst_vector, inst_vector_length);

  /* Allocate LOG struct and N log ringbuffer*/
  RLOG rlog;
  RINGBUFFER_TYPE *rb_log = (RINGBUFFER_TYPE *)malloc(sizeof(RINGBUFFER_TYPE));
  RINGBUFFER_VAR *rb_var = (RINGBUFFER_VAR *)malloc(sizeof(RINGBUFFER_VAR));
  ringbuffer_create(rb_log, (100 * sizeof(RLOG))); //100 log struct buffer
  ringbuffer_status(rb_log, rb_var);
  //check rb allocation
  //printf("rb_var.fill=%d\n", rb_var->fill);
  //printf("rb_var.free=%d\n", rb_var->free);
  //printf("rb_var.size=%d\n", rb_var->size);
    uint32_t num_inst = 0;
  
  /* Run the code until its end*/
  while (1) {
	uint32_t inst_raw = core_load(core, core->pc, 32);
    core->pc += 4;
	rlog.h_pc = core->pc;
	rlog.h_inst = inst_raw;
	rlog.mne[0] = 0;
    if (core->pc > inst_vector_length) break;
	core_execute(core, inst_raw, &rlog);
	num_inst++;
	ringbuffer_put(rb_log, &rlog, 0, sizeof(RLOG)); //put log struct in the ringbuffer
    if (core->pc == 0) break;
	//printf("num_inst=%d\n", num_inst);
  }
  
  /* Parse and stream ringbuffer log to disk*/
  FILE *flog = fopen("log.txt", "w"); 
  for (int i = 0; i < num_inst; i++) {
	ringbuffer_get(rb_log, &rlog, 0, sizeof(RLOG));
	fprintf(flog,"PC=%08x\n", rlog.h_pc);
	fprintf(flog,"[%08x]\n", rlog.h_inst);
	fprintf(flog,"x%02d=%08x\n", rlog.rd, rlog.h_rd);
	fprintf(flog,"x%02d=%08x\n", rlog.rs1, rlog.h_rs1);
	fprintf(flog,"x%02d=%08x\n", rlog.rs2, rlog.h_rs2);
	fprintf(flog,"%s\n", rlog.mne);
  }
  fclose(flog); 

  /* Deallocate CORE struct and its resources*/
  free(core->ram);
  free(core);
  ringbuffer_dispose(rb_log);
}
