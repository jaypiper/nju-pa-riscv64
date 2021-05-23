#include <stdio.h>
#include <sys/time.h>
#include <NDL.h>
int main() {
	NDL_Init(0);
	int count = 0;
	printf("finished\n");
	while (1) {
		long long time = NDL_GetTicks();
        	if(time / 500 >= count){
    			count ++;
    	 		printf("Hello from timer-test with count %d \n", count);
        	}
        }
        NDL_Quit();
  /*
  struct timeval tv;	
    long long time = gettimeofday(&tv, NULL);
    if(tv.tv_usec / 500000 >= count){
    	count ++;
    	printf("Hello from timer-test with count %d \n", count);
    }
  }
  */
  return 0;
}
