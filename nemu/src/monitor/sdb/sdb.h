#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);


////watchpoint
//#define NR_WP 33

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

  uint32_t val;
  char expr_str[32];
} WP;

//static WP wp_pool[NR_WP] = {};

void init_wp_pool();
WP* new_wp();
void free_wp(WP *wp);
bool check_wp();
void delete_wp(int num);
//static WP *head = NULL, *free_ = NULL;
void info_w();
#endif
