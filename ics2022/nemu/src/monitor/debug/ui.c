#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}
static int cmd_p(char *args){
  if(args==NULL)
  {
  	  printf("too few thing!");
 	return 1;
  }
char *arg = strtok(args," ");
int ins = expr(arg);
printf("%d",ins);
return 0;

}

static int cmd_w(char *args)
{
	char *arg = strtok(args," ");
	init_wp_pool();
	WP *p=new_wp();
	printf("Set watchpoint #%d\n",p->NO);
	strcpy(p->expr,arg);
	p->old_val = expr(arg);	

	if(p->old_val==p->new_val)
	
		printf("%s==1",p->expr);
	return 0;
}

static int cmd_d(char *args)
{
	char *arg = strtok(args," ");
	int N;
	 N=atoi(arg);
	 WP *p=head;
	 for(int i=1;i<N;i++)
	 {
		 p=p->next;
	 }
	 free_wp(p);
	 return 0;
}



static int cmd_help(char *args);

static int cmd_si(char *args){
	char *arg;
	int N=1;
	arg = strtok(args," ");
	if(arg==NULL)
	{
		N=1;
	}
	if(arg!=NULL) N=atoi(arg);
	cpu_exec(N);

	
	return 0;
}


static int cmd_info(char *args){
    char *arg = strtok(args," ");
    //cpu info
    if (strcmp(arg,"r")==0){
	    int i=0;
	    for(i=0;i<8;i++)
	    {
       printf("%s:\t%8x\t%d",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
    		printf("\n");
    	    }
	    }


    else if(strcmp(arg,"w")==0){
      printf("NO Expr  Old Value\n");
      WP *p=head;
      while(p!=free_)
      {
	      printf("%d %s %8x\n",p->NO,p->expr,p->old_val);
	      p=p->next;
      }

    }
     
    return 0;
} 

static int cmd_x(char *args){
    if(args == NULL){
        printf("too few things! \n");
        return 1;
    }
     
    char *arg = strtok(args," ");
    if(arg == NULL){
        printf("too few things! \n");
        return 1;
    }
    int  n = atoi(arg);
    char *EXPR = strtok(NULL," ");
    if(EXPR == NULL){                                                                                                                                          
        printf("too few things! \n");
        return 1;
    }
    if(strtok(NULL," ")!=NULL){
        printf(" many things! \n");
        return 1;
    }
    bool s = true;
    if (s!=true){
        printf("stop\n");
        return 1;
    }
    char *str;
    vaddr_t addr =  strtol( EXPR,&str,16 );
    for(int i = 0 ; i < n ; i++){
        uint32_t data = vaddr_read(addr+ i * 4,4);
        printf("0x%08x  " ,addr+i*4 );
        for(int j=0;j<4;j++){
            printf("0x%02x " , data & 0xff);
            data = data >> 8 ;
        }
        printf("\n");
    }
     
    return 0;
}    

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","-",cmd_si},
  { "info","+",cmd_info},
  { "x","*",cmd_x},
  { "p","/",cmd_p},
  { "d","=",cmd_d},
  { "w","%",cmd_w},
   /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
