#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
//#include <>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
//void init_wp_pool();

//void list_watchpoint();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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
 // Log("cmd c\n");
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
    exit(0);
        // return -1;
}

static int cmd_si(char *args){
    uint32_t num=0;
    if(args==NULL){
        num=1;
    }else{
        sscanf(args,"%u",&num);
    }
//    Log("num=%u\n",num);
    cpu_exec(num);

    return 0;
}

static int cmd_info(char *args){
    char *arg=strtok(NULL, " ");
    if (args==NULL){
        Log("usage: info [r|w]");
    }
	else {
    	if (strcmp(arg, "r") == 0) {
      		isa_reg_display();
    	}
    	else if (strcmp(arg, "w") == 0) {
      	   	info_w();
       	}else{
		    Log("usage: info [r|w]");  	
		}
	}
  return 0;
}

static int cmd_x(char *args) {
  /* extract the argument */
  char *N = strtok(NULL, " ");
  char *e = strtok(NULL, "\n");
  if (N == NULL || e == NULL) {
    // 参数错误
    printf("A syntax error in expression\n");
  } else {
    int n = atoi(N);

    // 求出表達式的值
    bool success;
    paddr_t base_addr = expr(e, &success);
    if (!success) {
      printf("Error expression!\n");
      return 0;
    }

    // 输出结果
    int i;
    for (i = 0; i < n; i++) {
      if (i % 4 == 0) {
        if (i != 0) printf("\n");
        printf("%#x:\t", base_addr);
      }
      printf("%#x\t", paddr_read(base_addr, 4));
      base_addr += 4;
    }
    printf("\n");
  }
  return 0;
}

static int cmd_p(char *args) {
  if (args != NULL) {
    bool success;
    word_t r = expr(args, &success);
    if(success) { printf("%d \n",r);printf(FMT_WORD "\n", r); }
    else { printf("Bad expression\n"); }
  }
  return 0;
}


static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");

  if (arg == NULL) {
    /* no argument given */
    Log("Delete watchpoint\nusage: d n");
  }
  else {
    int NO;
    sscanf(args, "%d", &NO);
    delete_wp(NO);
//     printf("delete watchpoint #%d\n",NO);

  }

  return 0;
}

static int cmd_w(char *args) {
  if (args != NULL) {
    bool cando=false;
    uint32_t value=expr(args,&cando);
   // Log("flag\n");
    if(cando==true){
        WP* wp=new_wp();
        if(wp!=NULL){
            strcpy(wp->expr_str, args);
            wp->val = value;
            printf("Set watchpoint #%d %s\nValue=%d   0x%08x \n", wp->NO,wp->expr_str,wp->val, wp->val);
        }
    }
   // int NO = set_watcpoint(args);
   // if (NO != -1) { printf("Set watchpoint #%d\n", NO); }
    else { printf("Bad expression!\n"); }
  }else{
    printf("usage: w expr\n");
  }
  return 0;
}



static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
//  { "w", "Set watchpoint", cmd_w },
//  { "d", "Delete watchpoint", cmd_d }, 
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step into implementation of N instructions after the suspension of execution. The default N is 1.", cmd_si},
  { "info", "info r - print register values; info w - show watch point state", cmd_info},
  { "x", "Examine memory", cmd_x },
  { "p", "Evaluate the value of expression", cmd_p },
  { "w", "Add watchpoints with a expression", cmd_w},
  { "d", "Remove watchpoint point",cmd_d}

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
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

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
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

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}

