#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
    
  int i;
  for (i = 0; i < 32; i ++) {
    printf("%s: 0x%08x ", regs[i], cpu.gpr[i]._32);
    if (i % 4 == 3) printf("\n");
  }
  printf("pc: 0x%08x\n", cpu.pc);
}

word_t isa_reg_str2val(const char *s, bool *success) {
	*success=true;
    for(int i=0;i<32;i++){
		if(strcmp(regs[i],s)==0) return cpu.gpr[i]._32;
	}
    if(strcmp("pc",s)==0) return cpu.pc;
    *success=false;
  return 0;
}
