#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main()
{
  struct timeval tv;
  struct tm time;
  int i=0;

  while(i<5)
  {
    gettimeofday(&tv,NULL);
    printf("sec:%ld\n",tv.tv_sec);
    sleep(1);
    i++;
  }

   time.tm_year=2016-1900;
   time.tm_mon=2;
   time.tm_mday=11;
   time.tm_hour=21;
   time.tm_min=46;
   time.tm_sec=0;
   
   tv.tv_sec=mktime(&time);
   printf("sec:%ld\n",tv.tv_sec);

   printf("setting time:%d",settimeofday(&tv,NULL));
}
