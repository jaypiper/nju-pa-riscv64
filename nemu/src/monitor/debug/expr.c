#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>
#include <memory/paddr.h>

word_t expr(char *e, bool *success);
uint64_t eval(int p, int q);
int get_priority(int p);
int parentheses_check(int p, int q);


enum {
  TK_NOTYPE = 256, TK_EQ, TK_UEQ,
  TK_AND, TK_OR,
  // TK_PLUS, TK_MINUS, TK_MULTI, TK_DIV, 
  TK_REG, TK_NUM, TK_HEX, TK_OCT,
  TK_LP, TK_RP,
  /* TODO: Add more token types */
  TK_LESS, TK_MORE
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"==", TK_EQ},        // equal
  {"!=", TK_UEQ},
  {"\\+", '+'},         // plus
  {"-", '-'},           // minus
  {"\\*", '*'},         // mul
  {"/", '/'},           // div
  {"\\$([a-z]|[0-9])+", TK_REG},
  {"0x([0-9]|[a-f])+", TK_HEX},
  {"0[0-7]+", TK_OCT},
  {"0|([1-9][0-9]*)", TK_NUM},
  {"\\(", TK_LP},
  {"\\)", TK_RP},
  {"<", TK_LESS},
  {">", TK_MORE}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

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

static Token tokens[128] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        assert(nr_token < 128 && substr_len <= 128);
        tokens[nr_token].type = rules[i].token_type;
        strncpy(tokens[nr_token].str, substr_start, substr_len);
        tokens[nr_token].str[substr_len] = 0;
        // nr_token ++;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        
        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          default: nr_token ++;
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

word_t expr(char *e, bool *success) {
  nr_token = 0;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token - 1);
}

void tokens_save(WP* wp){
  for(int i = 0; i < nr_token; i++){
    memcpy(&(wp->tokens[i]), &tokens[i], sizeof(Token));
  }
  wp->nr_token = nr_token;
}

void tokens_recover(WP* wp){
  for(int i = 0; i < wp->nr_token; i++){
    memcpy(&tokens[i], &(wp->tokens[i]), sizeof(Token));
  }
  nr_token = wp->nr_token;
}

uint64_t eval(int p, int q){
  assert(p <= q && q < nr_token);
  if(p == q) {
    if(tokens[p].type == TK_NUM || tokens[p].type == TK_OCT || tokens[p].type == TK_HEX){
      return strtoul(tokens[p].str, NULL, 0);
    }
    else if(tokens[p].type == TK_REG) {
      bool sucess = 0;
      uint64_t val = isa_reg_str2val(tokens[p].str, &sucess);
      assert(sucess);
      return val;
    }
  }
  else if(parentheses_check(p, q)) return eval(p+1, q-1);
  
  int last_id = p; // last op
  int depth = 0;
  for(int i = p; i <= q; i++){
    if(tokens[i].type == TK_LP) depth ++;
    else if(tokens[i].type == TK_RP) depth --;
    if(depth == 0 && get_priority(i) <= get_priority(last_id)) last_id = i;
  }
  if(last_id == p && tokens[p].type == '-' ) return 0 - eval(p+1, q);
  if(last_id == p && tokens[p].type == '*' ) return *((word_t*)guest_to_host(eval(p+1, q) - PMEM_BASE));
  uint64_t a = eval(p, last_id-1);
  uint64_t b = eval(last_id+1, q);
  switch(tokens[last_id].type){
    case '+': return a + b; break;
    case '-': return a - b; break;
    case '*': return a * b; break;
    case '/': return a / b; break;
    case TK_EQ: return a == b; break;
    case TK_UEQ: return a != b; break;
    case TK_LESS: return a < b; break;
    case TK_MORE: return a > b; break;
    default: printf("token should be an op: p: %d, q: %d, lastid: %d %s\n", p, q, tokens[last_id].type, tokens[last_id].str); assert(0);
  }
  assert(0);
}

int get_priority(int p){
  assert(0 <= p && p < nr_token);
  if((tokens[p].type == '-' || tokens[p].type == '*') && (p == 0 || get_priority(p-1) < 3)) return 5; // 前面存在其他运算符，为负数/解引用
  switch(tokens[p].type){
    case TK_EQ: case TK_UEQ: case TK_LESS: case TK_MORE: return 1;
    case '+': case '-': return 2;
    case '*': case '/': return 3;
  }
  // if(tokens[p].type == TK_EQ || tokens[p].type == TK_UEQ || tokens[p].type == TK_LESS || tokens[p].type == TK_MORE) return 1;
  // if((tokens[p].type == '-' || tokens[p].type == '*') && (p == 0 || get_priority(p-1) < 3)) return 5; // 前面存在其他运算符，为负数/解引用
  // if(tokens[p].type == '+' || tokens[p].type == '-') return 2;
  // else if(tokens[p].type == '*' || tokens[p].type == '/') return 3;
  // else if(tokens[p].type == '-' || tokens[p].type == '*') return 4;
  return 5;
}

int parentheses_check(int p, int q){
  if(tokens[p].type != TK_LP) return 0;
  int num = 1;
  for(int i = p+1; i <= q; i++){
    if(tokens[i].type == TK_LP) num ++;
    else if(tokens[i].type == TK_RP) num --;
    if(num == 0 && i != q) return 0;
    else if(num == 0) return 1;
  }
  assert(0); //括号不匹配
  return -1;
}

char buf[128];

void expr_test(){
  FILE *fp = fopen("tools/gen-expr/input", "r");
  assert(fp != NULL);
  word_t real_val;
  while(!feof(fp)){
    assert(fscanf(fp, "%lu", &real_val));
    for(int i = 0; i < 128; i++) buf[i] = 0;
    assert(fscanf(fp, "%[^\n]", buf));
    bool success = 0;
    if(!buf[0]) break;
    word_t val = expr(buf, &success);
    assert(success);
    if(val != real_val){
      printf("%s, %lu, expected: %lu\n", buf, val, real_val);
      assert(0);
    }
  }

  fclose(fp);

}