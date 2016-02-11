#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<signal.h>
#include<unistd.h>

#define LENGTH 100

void signal_handler(int signo)
{
  int len;
  char data[LENGTH];
  
  len=read(STDIN_FILENO,&data,LENGTH);
  data[len]=0;
  printf("string:%s\n",data);
}

int main()
{
  int flags;
  struct sigaction sigaction_struct;

  sigaction_struct.sa_handler =signal_handler;
  sigaction(SIGIO,&sigaction_struct,NULL);

  fcntl(STDIN_FILENO,F_SETOWN,getpid());

  flags=fcntl(STDIN_FILENO,F_GETFL);

  fcntl(STDIN_FILENO,F_SETFL,flags|FASYNC);

  while(1)
  {
    sleep(100);
  }
}
/*
 *string1
string:string1

I love you
string:I love you

hahah
string:hahah

^C

 * */
