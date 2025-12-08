#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "sdb.h"

int test_expr() {
  // Read input expression from pipeline
  // Format: result EXPR (for each line)
  char expressions[65536];
  if (fgets(expressions, sizeof(expressions), stdin) == NULL) {
    fprintf(stderr, "Failed to read expression from stdin\n");
    return 1;
  }

  // For each line, parse and evaluate the expression
  char *line = strtok(expressions, "\n");
  while (line != NULL) {
    char result_str[32];
    const char *space_pos = strchr(line, ' ');
    // Copy the result part
    if (space_pos == NULL) {
      fprintf(stderr, "Invalid input format: %s\n", line);
      return 1;
    }
    strncpy(result_str, line, space_pos - line);
    result_str[space_pos - line] = '\0';

    // The rest is the expression
    const char *expr_str = space_pos + 1;

    // Evaluate the expression
    bool success = false;
    word_t eval_result = expr((char *)expr_str, &success);
    if (!success) {
      fprintf(stderr, "Failed to evaluate expression: %s\n", expr_str);
      return 1;
    }

    Assert(eval_result == strtoul(result_str, NULL, 10),
           "Expression evaluation failed: %s", expr_str);
  }

  return 0;
}