#define _CONCAT(x, y) x##y
#define CONCAT(x, y) _CONCAT(x, y)
#define BITMASK(bits) ((1ull << (bits)) - 1)

// similar to x[hi:lo] in verilog
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1))
#define DEF_WIRE(name, w) uint64_t name : w
#define DEF_REG(name, w)                                                       \
  uint64_t name : w;                                                           \
  uint64_t CONCAT(name, _next) : w;                                            \
  uint64_t CONCAT(name, _update) : 1

#define EVAL(c, name, val)                                                     \
  do {                                                                         \
    c->CONCAT(name, _next) = (val);                                            \
    c->CONCAT(name, _update) = 1;                                              \
  } while (0)

#define UPDATE(c, name)                                                        \
  do {                                                                         \
    if (c->CONCAT(name, _update)) {                                            \
      c->name = c->CONCAT(name, _next);                                        \
    }                                                                          \
  } while (0)
