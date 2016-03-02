#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>


void thread1(void)
{
  printf("thread2 id:%d,real id:%d\n",getpid(),syscall(SYS_gettid));
  sleep(10);  
}

void thread2(void)
{
 printf("thread1 id:%d,real id:%d\n",getpid(),syscall(SYS_gettid));
 sleep(10);
}

int main(void)
{
  pthread_t id;
  int i,ret;
  ret=pthread_create(&id,NULL,(void*)thread1,NULL);
  if(ret!=0)
  {
   printf("Create pthread1 error!\n");
   return 1;
  }
  ret=pthread_create(&id,NULL,(void*)thread2,NULL);
  if(ret!=0)
  {
   printf("Create pthread2 erorr!\n");
   return 1;
  }
  printf("main thread id:%d,real id:%d\n",getpid(),syscall(SYS_gettid));
  sleep(10);
  pthread_join(id,NULL);
  return 0;
}
