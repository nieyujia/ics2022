#include "nemu.h"
#include "stdlib.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_s = 200,
  TK_NOTYPE = 256, 
  TK_EQ = 1,
  TK_NEQ = 2, 
  TK_sixteen = 16,
  TK_ten = 10,
  TK_machine = 9,
  TK_point = 3,
  TK_dif = 4,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" " ,TK_s}, 
  {" +", TK_NOTYPE},    // spaces
  {"0x[0-9,a-f]+",TK_sixteen},
  {"[0-9]+",TK_ten},
  {"\\$[a-z]{2,3}",TK_machine},
  {"\\(",'('},
  {"\\)",')'},
  {"\\*",'*'},
  {"\\/",'/'},
  {"\\+", '+'},  // plus
  {"\\-",'-'},
  {"==", TK_EQ},         // equal
  {"!=",TK_NEQ},

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX+1];

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
  char str[64];
} Token;

Token tokens[64];
int nr_token;


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

     
   
   

       Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
	printf("!");
        position =position+ substr_len;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
   if(i>=NR_REGEX)		continue;	

         switch (rules[i].token_type) {
          default:
					if(rules[i].token_type!=TK_NOTYPE){
            tokens[nr_token].type = rules[i].token_type;
            if(tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != TK_ten && tokens[i-1].type!= TK_sixteen && tokens[i-1].type!= TK_machine && tokens[i-1].type!=')')) ) 
                    tokens[i].type = TK_point;
            if(tokens[i].type == '-' && (i == 0 || (tokens[i - 1].type != TK_ten && tokens[i-1].type!= TK_sixteen && tokens[i-1].type!= TK_machine && tokens[i-1].type!=')')) )
                    tokens[i].type = TK_dif;

            strncpy(tokens[nr_token].str, substr_start,substr_len);
            nr_token++;
          }
				}

        break;
      }
    }


    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    
    }
    if(position>strlen(e)-1)	break;
  }


  return true;
}
 

bool check_parentheses(int p ,int q){
           int i,tag = 0;
	   if(p>q)	return false;	
         if(tokens[p].type != '(' || tokens[q].type != ')') return false;
                 for(i = p ; i <= q ; i ++){
                     if(tokens[i].type == '(') tag++;
       else if(tokens[i].type == ')') tag--;
        if(tag == 0 && i < q) return false ;
    }
    if( tag != 0 ) return false;
    return true;
}


            int dominant_operator(int p , int q){

    int i ,dom = p, left_n = 0;
    int pr = -1 ;
    for(i = p ; i <= q ; i++){
        if(tokens[i].type == 10){
            left_n += 1;
            i++;
            while(1){
                if(tokens[i].type == '(') left_n += 1;
                else if(tokens[i].type == ')') left_n --;
                i++;
                if(left_n == 0)
                    break;
            }
            if(i > q)break;
        }
        else if(tokens[i].type == 10) continue;
        else if((tokens[i].type ) > pr){
            pr = tokens[i].type;
            dom = i;
        }
    }
    return dom;
}


  uint32_t eval(int p, int q) {
	  
    if (p > q){
      return 0;
            /* Bad expression */
    }
    else if (p == q) {
        /* Single token.
        * For now this token should be a number.
        * Return the value of the number.
        */
            int d;
          if(tokens[p].type==16||tokens[p].type==10)
          {
                d = atoi(tokens[p].str);

                  return d;
          }
	  else
		  return false;
    }
    else if (check_parentheses(p, q) == true) {

        return eval(p + 1, q - 1);
    }
    else {
       
        int op = dominant_operator(p,q);
     int    val1 = eval(p, op - 1);
       int  val2 = eval(op + 1, q);
        switch (op) {
            case '+': return val1 + val2;
            case '-': return val1 - val2;
            case '*': return val1 * val2;
            case '/': return val1 / val2;
	    case 4: return -1*val2;
		case 3 : return val2;
            default: assert(0);
    }
}


  return 0;
}





uint32_t expr(char *e) {
make_token(e);
printf("已完成!");
int p = 0;
int q = nr_token-1 ;
int ins = eval(p,q);
return ins;
}
