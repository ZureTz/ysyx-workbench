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

#include "sdb.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_watchpoint_pool() {
  for (int i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

// Allocate a new watchpoint from free list
WP *new_wp() {
  if (free_ == NULL) {
    assert(0); // No free watchpoint available
  }

  WP *wp = free_;
  free_ = free_->next;
  wp->next = NULL;

  return wp;
}

// Free a watchpoint back to free list
void free_wp(WP *wp) {
  // Remove from head list (if it's there)
  if (head == wp) {
    head = head->next;
  } else {
    WP *p = head;
    while (p != NULL && p->next != wp) {
      p = p->next;
    }
    if (p != NULL) {
      p->next = wp->next;
    }
  }

  // Add to free list
  wp->next = free_;
  free_ = wp;
}

// Create a new watchpoint with expression
WP *create_watchpoint(const char *expr_str) {
  WP *wp = new_wp();
  strncpy(wp->expr, expr_str, sizeof(wp->expr) - 1);
  wp->expr[sizeof(wp->expr) - 1] = '\0';

  // Evaluate initial value
  bool success = false;
  wp->old_value = parse_and_evaluate(wp->expr, &success);
  if (!success) {
    free_wp(wp);
    return NULL;
  }

  // Add to head list
  wp->next = head;
  head = wp;

  return wp;
}

// Display all watchpoints
void display_watchpoints() {
  if (head == NULL) {
    printf("No watchpoints.\n");
    return;
  }

  printf("Num\tExpression\t\tValue\n");
  WP *p = head;
  while (p != NULL) {
    printf("%d\t%-20s\t" FMT_WORD "\n", p->NO, p->expr, p->old_value);
    p = p->next;
  }
}

// Delete watchpoint by number
bool delete_watchpoint(int no) {
  WP *p = head;
  while (p != NULL) {
    if (p->NO == no) {
      free_wp(p);
      return true;
    }
    p = p->next;
  }
  return false;
}

// Check all watchpoints and return true if any changed
bool check_watchpoints() {
  bool changed = false;
  WP *p = head;

  while (p != NULL) {
    bool success = false;
    word_t new_value = parse_and_evaluate(p->expr, &success);

    if (success && new_value != p->old_value) {
      printf("\nWatchpoint %d: %s\n", p->NO, p->expr);
      printf("Old value = " FMT_WORD "\n", p->old_value);
      printf("New value = " FMT_WORD "\n", new_value);
      p->old_value = new_value;
      changed = true;
    }

    p = p->next;
  }

  return changed;
}
