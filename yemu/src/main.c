#include "cpu.h"
#include "memory.h"

int main(int argc, char *const argv[]) {
  memory_init();

  while (1) {
    cpu_exec_once();
    if (halt) {
      break;
    }
  }

  printf("The result of 16 + 33 is %d\n", M[7]);
  return 0;
}
