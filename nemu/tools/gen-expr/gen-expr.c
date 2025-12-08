/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format = "#include <stdio.h>\n"
                           "int main() { "
                           "  unsigned result = %s; "
                           "  printf(\"%%u\", result); "
                           "  return 0; "
                           "}";

static int buf_pos = 0; // Current position in buf

// Generate a random number < n
static uint32_t choose(uint32_t n) { return rand() % n; }

// Append a string to buf with buffer overflow check
static void gen(const char *str) {
  int len = strlen(str);
  if (buf_pos + len >= 65536) {
    // Buffer overflow, stop generation
    buf_pos = 65536; // Mark as overflow
    return;
  }
  strcpy(buf + buf_pos, str);
  buf_pos += len;
}

// Generate random spaces (0-3 spaces)
static void gen_rand_space() {
  int n = choose(4); // 0, 1, 2, or 3 spaces
  for (int i = 0; i < n; i++) {
    gen(" ");
  }
}

// Generate a random number (unsigned)
static void gen_num() {
  char num_buf[32];
  // Avoid generating 0 to prevent division by zero
  uint32_t num = choose(99) + 1; // Generate numbers 1-99
  sprintf(num_buf, "%u", num);
  gen(num_buf);
}

// Generate a random operator
static void gen_rand_op() {
  char op_buf[2] = {0};
  switch (choose(4)) {
  case 0:
    op_buf[0] = '+';
    break;
  case 1:
    op_buf[0] = '-';
    break;
  case 2:
    op_buf[0] = '*';
    break;
  case 3:
    op_buf[0] = '/';
    break;
  }
  gen(op_buf);
}

// Internal recursive function to generate expressions
void gen_expr_internal(int depth) {
  // Prevent too deep recursion and buffer overflow
  if (depth > 10 || buf_pos >= 65536) {
    gen_num();
    return;
  }

  switch (choose(3)) {
  case 0:
    gen_num();
    break;
  case 1:
    gen("(");
    gen_rand_space();
    gen_expr_internal(depth + 1);
    gen_rand_space();
    gen(")");
    break;
  default: {
    gen_expr_internal(depth + 1);
    gen_rand_space();
    gen_rand_op();
    gen_rand_space();
    gen_expr_internal(depth + 1);
    break;
  }
  }
}

static void gen_rand_expr() {
  buf[0] = '\0';
  buf_pos = 0;

  gen_expr_internal(0);

  // If buffer overflow occurred, generate a simple expression instead
  if (buf_pos >= 65536) {
    buf[0] = '\0';
    buf_pos = 0;
    gen_num();
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0)
      continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
