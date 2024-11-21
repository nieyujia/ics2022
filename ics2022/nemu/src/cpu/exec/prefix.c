#include "cpu/exec.h"

make_EHelper(real);

make_EHelper(operand_size) {
  decoding.is_operand_size_16 = true;
  exec_real(eip);
  decoding.is_operand_size_16 = false;
}

make_EHelper(real) {
  uint32_t opcode = instr_fetch(eip, 1);
  decoding.opcode = opcode;
  set_width(opcode_table[opcode].width);
  idex(eip, &opcode_table[opcode]);
}
