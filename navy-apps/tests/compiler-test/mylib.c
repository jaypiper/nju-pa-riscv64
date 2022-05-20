#include <stdio.h>

int my_read(){
  printf("receive input 10\n");
  return 10;
}

int my_write(int n){
  printf("%d\n", n);
  return 0;
}