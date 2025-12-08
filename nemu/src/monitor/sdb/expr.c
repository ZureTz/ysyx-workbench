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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <readline/chardefs.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_DEC,
  TK_HEX,
  TK_NEG, // Negative operator (unary minus)

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},          // spaces
    {"\\+", '+'},               // plus
    {"-", '-'},                 // minus
    {"\\*", '*'},               // multiply
    {"/", '/'},                 // divide
    {"\\(", '('},               // left parenthesis
    {"\\)", ')'},               // right parenthesis
    {"0x[0-9a-fA-F]+", TK_HEX}, // hex number
    {"[0-9]+", TK_DEC},         // decimal number
    {"==", TK_EQ},              // equal
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  char error_msg[128];
  for (int i = 0; i < NR_REGEX; i++) {
    const int ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
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
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
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

  // Identify negative operators
  // (distinguish from minus, as it is identified as a TK_NEG)

  // A '-' is a negative operator if:
  // 1. It's at the beginning of the expression, OR
  // 2. The previous token is an operator or '('
  for (int i = 0; i < nr_token; i++) {
    // Filter out non '-' tokens
    if (tokens[i].type != '-') {
      continue;
    }

    // Check the conditions for negative operator

    // If '-' is the first token
    if (i == 0) {
      tokens[i].type = TK_NEG;
      continue;
    }

    // Check the previous token
    int prev_type = tokens[i - 1].type;
    if (prev_type == '+' || prev_type == '-' || prev_type == '*' ||
        prev_type == '/' || prev_type == '(' || prev_type == TK_EQ) {
      tokens[i].type = TK_NEG;
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
    if (tokens[i].type == '+' || tokens[i].type == '-') {
      precedence = 1;
    } else if (tokens[i].type == '*' || tokens[i].type == '/') {
      precedence = 2;
    } else if (tokens[i].type == TK_NEG) {
      precedence = 3; // Unary operators have higher precedence
    } else if (tokens[i].type == TK_EQ) {
      precedence = 0;
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
static word_t eval(int p, int q, bool *success) {
  if (p > q) {
    // Bad expression
    *success = false;
    return 0;
  }

  // Single token - should be a number
  if (p == q) {
    word_t num;
    if (tokens[p].type == TK_DEC) {
      sscanf(tokens[p].str, "%u", &num);
      return num;
    } else if (tokens[p].type == TK_HEX) {
      sscanf(tokens[p].str, "%x", &num);
      return num;
    } else {
      // Not a number
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
    return eval(p + 1, q - 1, success);
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

  // Handle unary negative operator
  if (tokens[op_pos].type == TK_NEG) {
    word_t val = eval(op_pos + 1, q, success);
    if (!*success)
      return 0;
    return -val;
  }

  // Recursively evaluate the left and right subexpressions
  word_t val1 = eval(p, op_pos - 1, success);
  if (!*success)
    return 0;

  word_t val2 = eval(op_pos + 1, q, success);
  if (!*success)
    return 0;

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
  default:
    *success = false;
    return 0;
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  *success = true;
  return eval(0, nr_token - 1, success);
}
