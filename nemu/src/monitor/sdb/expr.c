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

#include <isa.h>

// We use the POSIX regex functions to process regular expressions.
// Type 'man regex' for more information about POSIX regex functions.
#include <memory/vaddr.h>
#include <readline/chardefs.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_DEC,
  TK_HEX,
  TK_NEG,   // Negative operator (unary minus)
  TK_DEREF, // Pointer dereference (unary *)
  TK_REG,   // Register (starts with $)
  TK_NEQ,   // Not equal (!=)
  TK_AND,   // Logical AND (&&)
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
    {" +", TK_NOTYPE},           // spaces
    {"\\+", '+'},                // plus
    {"-", '-'},                  // minus
    {"\\*", '*'},                // multiply
    {"/", '/'},                  // divide
    {"\\(", '('},                // left parenthesis
    {"\\)", ')'},                // right parenthesis
    {"0x[0-9a-fA-F]+", TK_HEX},  // hex number
    {"[0-9]+", TK_DEC},          // decimal number
    {"==", TK_EQ},               // equal
    {"!=", TK_NEQ},              // not equal
    {"&&", TK_AND},              // logical AND
    {"\\$[a-zA-Z0-9]+", TK_REG}, // register (starts with $)
};

#define NR_REGEX ARRLEN(rules)

static regex_t compiled_regex_array[NR_REGEX] = {};

// Rules are used for many times.
// Therefore we compile them only once before any usage.
void init_regex() {
  char error_msg[128];
  for (int i = 0; i < NR_REGEX; i++) {
    const int ret =
        regcomp(&compiled_regex_array[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &compiled_regex_array[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&compiled_regex_array[i], e + position, 1, &pmatch, 0) == 0 &&
          pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
            rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        switch (rules[i].token_type) {
        case TK_NOTYPE:
          break;
        case TK_DEC:
        case TK_HEX:
        case TK_REG: // Also save register names
          if (substr_len >= 32) {
            printf("Token too long at position %d\n", position - substr_len);
            return false;
          }
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          tokens[nr_token].str[substr_len] = '\0';
        default:
          if (nr_token >= 32) {
            printf("Too many tokens at position %d\n", position);
            return false;
          }
          tokens[nr_token].type = rules[i].token_type;
          nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  // Identify negative operators and dereference operators
  // A '-' is a negative operator if it's at the beginning or after an
  // operator/parenthesis A '*' is a dereference operator if it's at the
  // beginning or after an operator/parenthesis
  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == '-' || tokens[i].type == '*') {
      bool is_unary = false;

      if (i == 0) {
        is_unary = true;
      } else {
        int prev_type = tokens[i - 1].type;
        // Unary if previous token is an operator or '('
        if (prev_type == '+' || prev_type == '-' || prev_type == '*' ||
            prev_type == '/' || prev_type == '(' || prev_type == TK_EQ ||
            prev_type == TK_NEQ || prev_type == TK_AND) {
          is_unary = true;
        }
      }

      if (is_unary) {
        if (tokens[i].type == '-') {
          tokens[i].type = TK_NEG;
        } else {
          tokens[i].type = TK_DEREF;
        }
      }
    }
  }

  return true;
}

// Check if the expression is surrounded by a matched pair of parentheses
// Returns: true if surrounded, false otherwise
// Sets success to false if parentheses are not balanced
static bool check_parentheses(int p, int q, bool *success) {
  if (tokens[p].type != '(' || tokens[q].type != ')') {
    return false;
  }

  int depth = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      depth++;
    } else if (tokens[i].type == ')') {
      depth--;
      if (depth == 0 && i < q) {
        // The first '(' matches with a ')' before the end
        return false;
      }
      if (depth < 0) {
        // More ')' than '('
        *success = false;
        return false;
      }
    }
  }

  if (depth != 0) {
    // Unbalanced parentheses
    *success = false;
    return false;
  }

  return true;
}

// Find the position of the main operator in the token expression [p, q]
// Returns: the position of the main operator, or -1 if not found
static int find_main_operator(int p, int q) {
  int main_op = -1;
  int min_precedence = 100; // Higher value = higher precedence
  int depth = 0;

  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      depth++;
      continue;
    }
    if (tokens[i].type == ')') {
      depth--;
      continue;
    }

    // Skip tokens inside parentheses
    if (depth > 0) {
      continue;
    }

    // Determine precedence (lower number = lower precedence = evaluated last)
    int precedence = 100;
    if (tokens[i].type == TK_AND) {
      precedence = 0; // Logical AND has lowest precedence
    } else if (tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ) {
      precedence = 1; // Equality operators
    } else if (tokens[i].type == '+' || tokens[i].type == '-') {
      precedence = 2;
    } else if (tokens[i].type == '*' || tokens[i].type == '/') {
      precedence = 3;
    } else if (tokens[i].type == TK_NEG || tokens[i].type == TK_DEREF) {
      precedence = 4; // Unary operators have higher precedence
    } else {
      // Not an operator
      continue;
    }

    // Update main operator: choose the one with lowest precedence
    // For operators with same precedence, choose the rightmost one
    // (left-to-right associativity)
    if (precedence <= min_precedence) {
      min_precedence = precedence;
      main_op = i;
    }
  }

  return main_op;
}

// Recursive evaluation of expression in tokens[p..q]
static word_t evaluate_tokens(int p, int q, bool *success) {
  if (p > q) {
    // Bad expression
    *success = false;
    return 0;
  }

  // Single token - should be a number or register
  if (p == q) {
    word_t num;
    if (tokens[p].type == TK_DEC) {
      sscanf(tokens[p].str, "%u", &num);
      return num;
    } else if (tokens[p].type == TK_HEX) {
      sscanf(tokens[p].str, "%x", &num);
      return num;
    } else if (tokens[p].type == TK_REG) {
      // Get register value (skip the '$' prefix)
      return isa_reg_str2val(tokens[p].str + 1, success);
    } else {
      // Not a valid token
      *success = false;
      return 0;
    }
  }

  // Expression is surrounded by parentheses
  if (check_parentheses(p, q, success)) {
    if (!*success) {
      // Parentheses are not balanced
      return 0;
    }
    // The expression is surrounded by a matched pair of parentheses
    // Remove the parentheses and evaluate the inner expression
    return evaluate_tokens(p + 1, q - 1, success);
  }

  if (!*success) {
    // check_parentheses detected unbalanced parentheses
    return 0;
  }

  // Find the main operator
  int op_pos = find_main_operator(p, q);
  if (op_pos == -1) {
    // No operator found - bad expression
    *success = false;
    return 0;
  }

  // Handle unary operators
  if (tokens[op_pos].type == TK_NEG) {
    word_t val = evaluate_tokens(op_pos + 1, q, success);
    if (!*success) {
      return 0;
    }
    return -val;
  }

  if (tokens[op_pos].type == TK_DEREF) {
    word_t addr = evaluate_tokens(op_pos + 1, q, success);
    if (!*success) {
      return 0;
    }
    // Read 4 bytes (word_t size) from memory at address
    return vaddr_read(addr, sizeof(word_t));
  }

  // Recursively evaluate the left and right subexpressions
  word_t val1 = evaluate_tokens(p, op_pos - 1, success);
  if (!*success) {
    return 0;
  }

  word_t val2 = evaluate_tokens(op_pos + 1, q, success);
  if (!*success) {
    return 0;
  }
  // Apply the operator
  switch (tokens[op_pos].type) {
  case '+':
    return val1 + val2;
  case '-':
    return val1 - val2;
  case '*':
    return val1 * val2;
  case '/':
    if (val2 == 0) {
      printf("Division by zero\n");
      *success = false;
      return 0;
    }
    return val1 / val2;
  case TK_EQ:
    return val1 == val2;
  case TK_NEQ:
    return val1 != val2;
  case TK_AND:
    return val1 && val2;
  default:
    *success = false;
    return 0;
  }
}

word_t parse_and_evaluate(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  // Default to success
  // (will be changed in evaluate_tokens() if any error occurs)
  *success = true;
  return evaluate_tokens(0, nr_token - 1, success);
}
