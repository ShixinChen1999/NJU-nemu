#include <isa.h>
#include <debug.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>


/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

//uint32_t GetMarkValue(char* str, bool* success );
enum {
  TK_NOTYPE = 256, 
  TK_EQ,
  TK_DECINT, 
  TK_HEXINT,
  TK_REG,
  TK_DEREF, 
  TK_UNEQ, 
  TK_LOGICAND,
  TK_LOGICOR,
  TK_LOGICNOT,
  TK_MINUS,
  TK_MARK
    
          /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"\\b0[xX}[0-9a-fA-F]+\\b", TK_HEXINT},   // hexadecimal number
  {"\\b[0-9]+\\b", TK_DECINT},   //十进制整数 Decimal integer
  {"-", '-'},           // subtraction
  {"\\*", '*'},         // multiply
  {"/", '/'},           // divide
  {"\\(", '('},         // Left parenthesis
  {"\\)", ')'},         // Right parenthesis
  {"!=", TK_UNEQ},         // unequal
  {"\\$[a-zA-Z_][a-zA-Z0-9_]*", TK_REG},         // reg_name
  {"&&", TK_LOGICAND},         // logic and
  {"\\|\\|", TK_LOGICOR},
  {"!",TK_LOGICNOT},
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\b[a-zA-Z0-9_]*\\b",TK_MARK}                   //mark
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
static bool* cando; //the flag for continuely oparating when meeting errors

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

      //  Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
      //       i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
                case TK_NOTYPE: break;
               // case TK_REG: TODO();
                default: {
                   tokens[nr_token].type=rules[i].token_type;
                  // printf("%ld\n",strlen(rules[i].regex);
                 // if(tokens[nr_token].type==TK_REG)
                   if(substr_len>32){
                        printf("The length of expressions exceeds the max capacity!\n");
                        printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
                        return false;
                   }
                            //check if the length exceeds the max length
                   strncpy(tokens[nr_token].str,substr_start,substr_len);
                   tokens[nr_token].str[substr_len]='\0';
                // Log("tokens[%d].token_type=%d  str=%s",nr_token,tokens[nr_token].type, tokens[nr_token].str);
                   nr_token++;
                  }// TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


bool check_parentheses(int p,int q){
     // Log("check ( )\n bwtween %d and %d\n",p,q);
      int lc=0,rc=0;
       if((tokens[p].type=='(')&&(tokens[q].type==')')){
            for(int i=p+1;i<q;i++){
                if(tokens[i].type=='('){lc++;}
                if(tokens[i].type==')'){rc++;}
                if(rc>lc) return false;
            }
            if(rc==lc){
       //         Log("check_parenttheses success\n");
                return true;
            }
       }
       else{
          return false;
       }

       return false;
}


//int check_main_op(int p,int q){//for simple expr
//
//        int main_op=0; 
//        bool op_plus_or_minus_flag=false;
//        int nr=0;//number of the parentheses
//           for(int i=p;i<q;i++){
//               if(tokens[i].type=='('){
//                nr++;      
//               }else if(tokens[i].type==')'){
//                nr--;
//               }
//           //      printf("nr=%d\n",nr);
//
//              if(nr==0){
//                switch(tokens[i].type){
//                    case '+': 
//                    case '-':{
//                              main_op=i; 
//                              op_plus_or_minus_flag=true;
//                              break;
//                             }
//                    case '*':
//                    case '/':{
//                              if(op_plus_or_minus_flag==false){
//                                main_op=i;
//                              }
//                              break;
//                             }
//                   default: break;//printf("default!\n");
//                 }   
//
//            }
//
//    }
//
//    // Log("The main op between %d and %d is at %d %d\n",p,q,main_op,tokens[main_op].type);   
//     return main_op; 
//
//}


int check_main_op(int p, int q){
   // if(*cando==false) {return 0;}
     int nr_brack=0;
     int priority=0;
     int main_op=0;
     for(int i=p;i<q;i++){
        if(tokens[i].type=='('){ nr_brack++;}
        if(tokens[i].type==')'){ nr_brack--;}

        if(nr_brack!=0){ continue;}
        switch(tokens[i].type){
            case '+'://pri=5
            case '-':{
                if(priority<4) {main_op=i;}
                priority=4;
                break;
               }
            case '*'://pri=4
            case '/':{
                if(priority<3) {main_op=i;}
                priority=3;
                break;
               }
            case TK_MINUS://pri=2
            case TK_DEREF:
            case TK_LOGICNOT:{
                if(priority<2) {main_op=i;}
                priority=2;
                break;
            }
            case TK_LOGICAND:{//pri=12
                if(priority<12){main_op=i;}
                priority=12;
                break;
                 }
            case TK_LOGICOR:{//pri=13
                if(priority<13){main_op=i;}
                    priority=13;
                    break;
             }
            case TK_EQ:
            case TK_UNEQ:{//pri=8
                if(priority<8){main_op=i;}
                    priority=8;
                    break;
                }
            default: break;
        }
   }
// Log("The main op between %d and %d is at %d %d\n",p,q,main_op,tokens[main_op].type); 
 if(priority==0) {*cando=false; return 0;}
      
 return main_op;

}
uint32_t eval(int p, int q){
  //Log("eval between %d and %d\n",p,q);
        // Log("beginning the evaluation\n");
   if(*cando==false){ return 0;} 
   if (p > q) {
     printf("Bad expression!!\n"); 
     *cando=false;
     return 0;
    // return 0; 
     // exit(0);//TODO: Add continue operation.
     /* Bad expression */
   }
  else if (p == q) {
     uint32_t num;
  //   bool   cando;
     switch(tokens[p].type){//four types need to tranformed as decimal number
        case TK_DECINT: {
                            sscanf(tokens[p].str,"%d",&num);
                            return num;
                        }
        case TK_HEXINT: {
                            sscanf(tokens[p].str,"%x",&num);
                            return num;
                        }
        case TK_REG:    {
                         num=isa_reg_str2val((tokens[p].str+1),cando);//detached $
                         if(*cando==true){return num;}else{printf("wrong register name\n"); return 0;}
                        }
        case TK_MARK:   {
                         TODO(); // num = GetMarkValue(tokens[p].str, &cando);
                         if(*cando==true){return num;}else{printf("can not access %s, please check the variable name\n",tokens[p].str); return 0;}
                        } 
        default: {printf("wrong number in %d\n",p); *cando=false; return 0;}

     }
          /* Single token.
       *      * For now this token should be a number.
       *           * Return the value of the number.
       *                */
    }
  else if (check_parentheses(p, q) == true) {
      /* The expression is surrounded by a matched pair of parentheses.
       *      * If that is the case, just throw away the parentheses.
       *           */
      return eval(p + 1, q - 1);
    }
  else {
      int op = check_main_op(p,q);// the position of 主运算符 in the token expression;
     // int op_type='+';
    //  Log("main op = %d\n",op);
      if (p == op || tokens[p].type == TK_LOGICNOT || tokens[p].type == TK_MINUS || tokens[p].type == TK_DEREF) {
			uint32_t temp_num = eval(p + 1, q);
			switch (tokens[p].type) {
			case TK_DEREF: return paddr_read(temp_num, 4);
			case TK_LOGICNOT: return !temp_num;
			case TK_MINUS: return -temp_num;
			default: { *cando=false; return 0;}
			}
		}//The expr supports the *(expr) -(expr) operation
      
      uint32_t val1 = eval(p, op - 1);
      uint32_t val2 = eval(op + 1, q);

      switch (tokens[op].type) {
            case '+': return val1 + val2;
            case '-': return val1 - val2;
            case '*': return val1 * val2;
            case '/': return val1 / val2; 
            case TK_EQ:  return val1 == val2;
			case TK_UNEQ: return val1 != val2;
			case TK_LOGICAND: return val1 && val2;
			case TK_LOGICOR:  return val1 || val2; 
            default: *cando=false; return 0;
          }
    }
 
}



word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  cando=success;

  int nr_brack=0;
for (int i = 0; i < nr_token; i ++) {
  if(tokens[i].type == '('){nr_brack++;}
  if(tokens[i].type == ')'){nr_brack--;}
  if(nr_brack<0){
    *success = false;
    return 0;
  }//expr error
  if (tokens[i].type == '*' && (i == 0 ||( (tokens[i-1].type != ')') && (tokens[i - 1].type != TK_DECINT) && (tokens[i-1].type!=TK_HEXINT) && (tokens[i-1].type!=TK_REG))) ) {
    tokens[i].type = TK_DEREF;
  }
  if (tokens[i].type == '-' && (i == 0 ||( (tokens[i-1].type != ')') && (tokens[i - 1].type != TK_DECINT) && (tokens[i-1].type!=TK_HEXINT) && (tokens[i-1].type!=TK_REG))) ) {
    tokens[i].type = TK_MINUS;
  }

}
if(nr_brack != 0){
    *success=false;
    return 0;
}//bracks are not matching

  
  /* TODO: Insert codes to evaluate the expression. */
//  TODO();
  *success=true;

  int p=0;
  int q=nr_token-1;
//  Log("before  success= %d\n",*success);
 // uint32_t res= eval(p,q);
  // *success = true; 

 // Log("after success= %d\n",*success);
 // *success = true; 

  return eval(p,q);
}
