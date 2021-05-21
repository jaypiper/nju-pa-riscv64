#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  struct watchpoint *pre;
  /* TODO: Add more members if necessary */
  char watch_inst[64];
  word_t pre_val;
} WP;

WP* new_wp();
void free_wp(WP *wp);
void watchpoint_info();
WP* find_wp(int id);
bool check_watchpoint();
#endif
