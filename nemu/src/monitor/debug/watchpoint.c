#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  // num = 0;
  int i;
  for (i = 0; i < NR_WP ; i ++) wp_pool[i].NO = i;
  for (i = 0; i < NR_WP - 1; i ++) {
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i+1].pre = &wp_pool[i];
  }
  wp_pool[0].pre = NULL;
  
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(char* args){
  assert(free_);
  WP* selected = free_;
  free_ = free_->next;
  if(free_) free_->pre = NULL;
  selected->pre = selected->next = NULL;

  strcpy(selected->watch_inst, args);
  bool success = 0;
  selected->pre_val = expr(args, &success);
  if(!success){
    printf("invalid expr\n");
    return 0;
  }
  tokens_save(selected);

  selected->next = head;
  if(head) head->pre = selected;
  head = selected;
  return selected;
}

WP* find_wp(int id){
  
  for(WP* iter = head; iter; iter = iter->next){
    if(iter->NO == id) return iter;
  }
  assert(0);
}

void free_wp(WP *wp){
  assert(wp);
  if(wp->pre) wp->pre->next = wp->next;
  else head = wp->next;
  if(wp->next) wp->next->pre = wp->pre;
  
  if(free_) free_->pre = wp;
  wp->next = free_;
  wp->pre = NULL;
  free_ = wp;
}

/* TODO: Implement the functionality of watchpoint */

void watchpoint_info(){
  printf("NO      Inst\n");
  for(WP* iter = head; iter; iter = iter->next){
    printf("%3d     %s\n", iter->NO, iter->watch_inst);
  }
}

bool check_watchpoint(){
  bool is_change = false;
  for(WP* iter = head; iter; iter = iter->next){
    // bool success = 0;
    tokens_recover(iter);
    // word_t cur_val = expr(iter->watch_inst, &success);
    word_t cur_val = eval(0, iter->nr_token - 1);
    if(cur_val != iter->pre_val){
      printf("Breakpoint %d, %s, pre-val: %lu  %lx,  cur-val: %lu  %lx\n", iter->NO, iter->watch_inst, iter->pre_val, iter->pre_val, cur_val, cur_val);
      iter->pre_val = cur_val;
      is_change = true;
    }
  }
  return is_change;
}