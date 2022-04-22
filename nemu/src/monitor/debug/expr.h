#ifndef __EXPR_H__
#define __EXPR_H__

#include <common.h>

typedef struct token {
  int type;
  char str[128];
} Token;

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  struct watchpoint *pre;
  /* TODO: Add more members if necessary */
  char watch_inst[64];
  Token tokens[64];
  int nr_token;
  word_t pre_val;
} WP;

WP* new_wp();
void free_wp(WP *wp);
void watchpoint_info();
WP* find_wp(int id);
bool check_watchpoint();

word_t expr(char *, bool *);
uint64_t eval(int p, int q);
void tokens_save(WP* wp);
void tokens_recover(WP* wp);


#endif
