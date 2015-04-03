/*sample: to demonstrate fork() and execl() and system calls
 *
 ***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main( int argc, char *argv[], char *env[] )
{
   pid_t my_pid, parent_pid, child_pid; 
   pid_t child_pid0;
   int status;

/* get and print my pid and my parent's pid. */

   my_pid = getpid();    parent_pid = getppid();
   printf("\n Parent: my pid is %d\n\n", my_pid);
   printf("Parent: my parent's pid is %d\n\n", parent_pid);


/* print error message if fork() fails */
   if((child_pid = fork()) < 0 )
   {
      perror("fork failure");
      exit(1);
   }

/* second child: print error message if fork() fails */

   else if((child_pid0 = fork()) < 0 )
   {
      perror("fork failure");
      exit(1);
   }


/* fork() == 0 for child process */

   if(child_pid == 0)
   {  printf("\nChild: I am a new-born process!\n\n");
      my_pid = getpid();    parent_pid = getppid();
      printf("Child: my pid is: %d\n\n", my_pid);
      printf("Child: my parent's pid is: %d\n\n", parent_pid);
      printf("Child: I will sleep 3 seconds and then execute the child executable \n\n");

      sleep(3); 
      printf("Child: Now, I woke up and am executing the child executable \n\n");
      execl("./hello", "hello", 0, (char* )0);
      //execl("/bin/date", "date", 0, (char*) NULL)
      //execl( "/bin/ls", "ls", "-l", (char*)0 );
      perror("execl() failure!\n\n");

      printf("This print is after execl() and should not have been executed if execl were successful! \n\n");

      _exit(1);
   }


/*
 * parent process
 */
   else
   {
	   /* fork() == 0 for child process number 2*/

	   if(child_pid0 == 0)
	   {  printf("\nChild 2: I am a new-born process that is a twin!\n\n");
		   //my_pid = getpid();    parent_pid = getppid();
		   printf("Child 2: my pid is: %d\n\n", my_pid);
		   printf("Child 2: my parent's pid is: %d\n\n", parent_pid);
		   printf("Child 2: I will sleep 5 seconds and then execute the child executable \n\n");

		   sleep(3); 
		   printf("Child 2: Now, I woke up and am executing the child executable \n\n");
		   execl("./hell0", "hell0", 0, (char* )0);
		   perror("execl() failure!\n\n");

		   printf("This print is after execl() and should not have been executed if execl were successful! \n\n");

		   _exit(1);
	   }
	   else{

	   printf("\nParent: I created a child process.\n\n");
	   printf("Parent: my child's pid is: %d\n\n", child_pid);
	   printf("Parent: my other child's pid is: %d\n\n", child_pid0);
	   //system("ps -acefl | grep ercal");  printf("\n \n");
	   wait(NULL); /* can use wait(NULL) wait(&status) since exit status
			  from child is not used. */
	   printf("\n Parent: my child is dead. I am going to leave.\n \n ");
	   }
   }

   return 0;
}

