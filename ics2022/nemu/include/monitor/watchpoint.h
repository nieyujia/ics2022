#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__
#include "common.h"

#define NR_WP 32
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[32];
  int new_val;
  int old_val;

} WP;


static WP *head, *free_;



void init_wp_pool();

WP* new_wp();
void free_wp(WP *wp);

            
#endif
