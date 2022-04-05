#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

//static void gen_rand_expr() {
//  buf[0] = '\0';
//}

static char *pbuf;

#define format_buf(fmt, ...) pbuf += sprintf(pbuf, fmt, ##__VA_ARGS__)

static inline uint32_t choose(uint32_t max) {
          return rand() % max;
}

static inline void gen_rand_op() {
  char op_list[] = {'+', '-', '*', '/', '+', '-', '*'};
  format_buf("%c", op_list[choose(7)]);
}

static inline void gen_num() {
  format_buf("%uu", rand());
}

static inline void gen_space() {
  char *space_list[3] = {
    "",
    " ",
    "  ",
  };
  format_buf("%s", space_list[choose(3)]);
}

static int nr_op = 0;

static inline void gen_rand_expr() {
  gen_space();
  switch (choose(3)) {
    default:
      if (nr_op == 0) gen_rand_expr();
      else gen_num();
      break;
    case 1:
      format_buf("(");
      gen_rand_expr();
      format_buf(")");
      break;
    case 0:
      nr_op ++;
      if (pbuf - buf >= sizeof(buf) / 2) {
        gen_num();
        break;
      }
      gen_rand_expr();
      gen_space();
      gen_rand_op();
      gen_space();
      gen_rand_expr();
      break;
  }
  gen_space();
}


int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  printf("%d\n",loop);
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    printf(code_format,buf);
    sprintf(code_buf, code_format, buf);
    FILE *fp = fopen("/home/sxchen/ics2021/nemu/tools/gen-expr/.code.c", "w");
//    FILE *fp = fopen("/tmp/.code.c", "w");
//    printf("fp=%d\n",*fp);
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /home/sxchen/ics2021/nemu/tools/gen-expr/.code.c -o /home/sxchen/ics2021/nemu/tools/gen-expr/.expr");
    if (ret != 0) continue;

    fp = popen("/home/sxchen/ics2021/nemu/tools/gen-expr/.expr", "r");
    assert(fp != NULL);

    int result;
    if( fscanf(fp, "%d", &result)!=1){
        printf("Writing the results to file error!\n");
    }
    //fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
