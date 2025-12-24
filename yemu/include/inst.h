#ifndef __INST_H__
#define __INST_H__

#include "common.h"

// 定义指令格式
typedef union {
  struct {
    uint8_t rs : 2, rt : 2, op : 4;
  } rtype;
  struct {
    uint8_t addr : 4, op : 4;
  } mtype;
  uint8_t inst;
} inst_t;

// 指令集架构
//                                                      4  2  0
//             |                        |        | +----+--+--+
// mov   rt,rs | R[rt] <- R[rs]         | R-type | |0000|rt|rs|
//             |                        |        | +----+--+--+
//             |                        |        | +----+--+--+
// add   rt,rs | R[rt] <- R[rs] + R[rt] | R-type | |0001|rt|rs|
//             |                        |        | +----+--+--+
//             |                        |        | +----+--+--+
// load  addr  | R[0] <- M[addr]        | M-type | |1110| addr|
//             |                        |        | +----+--+--+
//             |                        |        | +----+--+--+
// store addr  | M[addr] <- R[0]        | M-type | |1111| addr|
//             |                        |        | +----+--+--+

// Instruction decode functions (replacing macros)
typedef struct {
  uint8_t rt;
  uint8_t rs;
} decoded_rtype_t;

typedef struct {
  uint8_t addr;
} decoded_mtype_t;

static inline decoded_rtype_t decode_rtype(inst_t inst) {
  decoded_rtype_t result;
  result.rt = inst.rtype.rt;
  result.rs = inst.rtype.rs;
  return result;
}

static inline decoded_mtype_t decode_mtype(inst_t inst) {
  decoded_mtype_t result;
  result.addr = inst.mtype.addr;
  return result;
}

#endif
