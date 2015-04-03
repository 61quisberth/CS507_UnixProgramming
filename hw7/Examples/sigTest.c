#include <unistd.h>
#include <stdio.h>
#include <signal.h>
 
int v[3] = {0, 0, 0};
 
static void handler(int sig)
{
  v[0]++;
}
 
int main()
{
  int pid, pid1, pid2;
  signal(SIGUSR1, handler);
 
  pid = getpid();
  pid1 = fork();
   
  if (pid1) 
	pause();
  else 
	sleep(1);
 
  pid2 = fork();
 
  if (pid2) 
	sleep(1);
  if (pid1 == 0)  
	v[1]++;
  if (pid2 == 0) 
	v[2]++;
  if (getppid() == pid) 
	kill(pid, SIGUSR1);
 
  printf("%d %d %d pid: %d\n", v[0], v[1], v[2], getpid());
  return 0;
}
