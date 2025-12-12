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

#include <stdlib.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "sdb.h"
#include <cpu/cpu.h>
#include <isa.h>
#include <memory/vaddr.h>

static int is_batch_mode = false;

void init_regex();
void init_watchpoint_pool();

/* We use the `readline' library to provide more flexibility to read from stdin.
 */
static char *retrieve_command() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int command_continue(char *args) {
  cpu_exec(-1);
  return 0;
}

static int command_quit(char *args) { return -1; }

static int command_help(char *args);

static int command_step_instructions(char *args);

static int command_info(char *args);

static int command_scan(char *args);

typedef struct {
  const char *name;
  const char *description;
  int (*handler)(char *);
} command_props_t;

static command_props_t command_table[] = {
    {"help", "Display information about all supported commands", command_help},
    {"c", "Continue the execution of the program", command_continue},
    {"q", "Exit NEMU", command_quit},
    {"si", "Halt NEMU after executing N instructions",
     command_step_instructions},
    {"info", "Display the state of registers or watchpoints (r|w)",
     command_info},
    {"x", "Scan memory", command_scan},
};

#define LEN_COMMANDS ARRLEN(command_table)

static int command_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < LEN_COMMANDS; i++) {
      printf("%s - %s\n", command_table[i].name, command_table[i].description);
    }
  } else {
    // Has argument given
    for (i = 0; i < LEN_COMMANDS; i++) {
      if (strcmp(arg, command_table[i].name) == 0) {
        printf("%s - %s\n", command_table[i].name,
               command_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

// Single step execution for given number of instructions
static int command_step_instructions(char *args) {
  /* extract the first argument */
  const char *arg = strtok(NULL, " ");

  // If no argument given, default to 1
  int n = 1;

  // If argument given, parse it as an integer
  if (arg != NULL) {
    n = atoi(arg);
  }

  // Execute n instructions
  cpu_exec(n);

  return 0;
}

// Display information about registers or watchpoints
static int command_info(char *args) {
  // extract the first argument
  const char *arg = strtok(NULL, " ");

  // Return if argument is invalid
  if (arg == NULL || (strcmp(arg, "r") != 0 && strcmp(arg, "w") != 0)) {
    printf("Invalid argument '%s'; Usage: info r|w\n", arg);
    return 0;
  }

  // Display registers
  if (strcmp(arg, "r") == 0) {
    isa_reg_display();
    return 0;
  }

  // Otherwise, display watchpoints
  // wp_display();

  return 0;
}

// Scan memory and display contents (s N EXPR)
static int command_scan(char *args) {
  char *n_str = strtok(NULL, " ");
  if (n_str == NULL) {
    printf("Usage: x N EXPR\n");
    return 0;
  }
  int n = atoi(n_str);

  char *expr_str = strtok(NULL, "");
  if (expr_str == NULL) {
    printf("Usage: x N EXPR\n");
    return 0;
  }

  // Evaluate the expression to get the starting address
  bool success = false;
  vaddr_t addr = parse_and_evaluate(expr_str, &success);

  if (!success) {
    printf("Invalid expression: %s\n", expr_str);
    return 0;
  }

  for (int i = 0; i < n; i++) {
    word_t val = vaddr_read(addr + i * 4, 4);
    printf(FMT_WORD ": " FMT_WORD "\n", addr + i * 4, val);
  }
  return 0;
}

void sdb_set_batch_mode() { is_batch_mode = true; }

void sdb_mainloop() {
  if (is_batch_mode) {
    command_continue(NULL);
    return;
  }

  for (char *str; (str = retrieve_command()) != NULL;) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < LEN_COMMANDS; i++) {
      if (strcmp(cmd, command_table[i].name) == 0) {
        if (command_table[i].handler(args) < 0) {
          return;
        }
        break;
      }
    }

    if (i == LEN_COMMANDS) {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_watchpoint_pool();
}
