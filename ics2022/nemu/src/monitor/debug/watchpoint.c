#include "monitor/watchpoint.h"
#include "monitor/expr.h"


#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


WP* new_wp();
void free_wp(WP *wp);


WP* new_wp()
{
        if(head==NULL)
        {
		init_wp_pool();
                free_=free_->next;
                head = wp_pool;
                return head;
        }
        else
        {
                if(free_==NULL)
                {
                        printf("检视点已满！");
                        assert(0);
                }
                        WP *p=free_;
                free_ = free_->next;
                return p;
        }

}

void free_wp(WP *wp)
{
        WP *p=head;
        for(;;)
        {
                if(p->next==wp)
                        break;
                else
                        p=p->next;
        }
        p->next=wp->next;
        wp->next=free_->next;
        free_->next=wp;
}




int set_watchpoint(char *e);    //给予一个表达式e，构造以该表达式为监视目标的监视点，并返回编号
bool delete_watchpoint(int NO); //给予一个监视点编号，从已使用的监视点中归还该监视点到池中
void list_watchpoint(void);     //显示当前在使用状态中的监视点列表
WP* scan_watchpoint(void);      //扫描所有使用中的监视点，返回触发的监视点指针，若无触发返回NULL






int set_watchpoint(char *e)
{
	WP *p=new_wp();
	strcpy(p->expr,e);
	return p->NO;
}

bool delete_watchpoint(int NO)
{
	WP *p=head;
	for(int i=0;i<NO;i++)
	{
		p=p->next;
	}
	free_wp(p);
return 0;
}

void list_watchpoint(void)
{
 WP *p=head;
        while(p!=free_)
        {
		printf("%d  %s %x  %X",p->NO,p->expr,p->old_val,p->new_val);
                p=p->next;
        }


}


WP* scan_watchpoint()
{
 WP *p=head;
while(p!=free_)
{
if(p->old_val==p->new_val)
return p;
}
return NULL;
}	




















