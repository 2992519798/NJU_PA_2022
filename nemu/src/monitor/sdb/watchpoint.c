/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32
#include "watchpoint.h"

static WP wp_pool[NR_WP] = {};
static WP *wp_head = NULL, *wp_tail = NULL,           // watch point list
          *free_ = NULL, *free_tail = NULL;           // free nodes list

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }
  WP dummyhead;
  wp_head = &dummyhead;
  wp_tail = wp_head;
  free_ = wp_pool;
  free_tail = wp_pool + NR_WP;
}

/* TODO: Implement the functionality of watchpoint */

void free_wp(int no){
  WP* pre = wp_head;
  while (pre->next && pre->next->NO != no) {
    pre = pre->next;
  }
  if (pre->next) {
    WP *wp = pre->next;              // search wp successfully
    pre->next = wp->next;
    free_tail->next = wp;
    free_tail = wp;
    wp->next = NULL;
  } else {
    panic("Fail to free!\n");
  }
}

WP* new_wp() {
  if (free_) {
    WP *new = free_;
    if (free_->next) {
      free_ = free_->next;
    }
    new->next = NULL;
    wp_tail->next = new;
    wp_tail = new;
    return new;
  } else {
    panic("No more free watch point nodes in wp_pool!\n");
    return free_;
  }
}

bool check_wp() {
  if (!wp_head) {
    return false;
  }
  WP *cur = wp_head;
  bool success = true;
  while (cur) {
    if (cur->val != expr(cur->args, &success)) {
      if (!success) {
      printf("Bad expression,try again.\n");
      return false;
      }
      printf("NO. %d watchpoint's value changed.\n", cur->NO);
      return true;
    }
    cur = cur->next;
  }
  return false;
}

void watchpoint_display() {
  if (!wp_head->next) {
    printf("No watchpoint.\n");
  } else {
    WP *cur = wp_head->next;
    while (cur) {
      printf("NO.%d watchpoint\n", cur->NO);
      cur = cur->next;
    }
  }
}
