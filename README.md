# Linux_Pipes

Task: 1  

Here’s the skeleton of a shell function implementing a simple two-command pipeline, 

such as “cmd1 | cmd2”.

 

void simple_pipe(const char* cmd1, char* const* argv1, const char* cmd2, char* const* argv2) {
   int pipefd[2], r, status;

   [A]

   pid_t child1 = fork();
   if (child1 == 0) {
       [B]
       execvp(cmd1, argv1);
   }
   assert(child1 > 0);

   [C]

   pid_t child2 = fork();
   if (child2 == 0) {
       [D]
       execvp(cmd2, argv2);
   }
   assert(child2 > 0);

   [E]
}

And here is a grab bag of system calls.

[1] close(pipefd[0]);
[2] close(pipefd[1]);
[3] dup2(pipefd[0], STDIN_FILENO);
[4] dup2(pipefd[0], STDOUT_FILENO);
[5] dup2(pipefd[1], STDIN_FILENO);
[6] dup2(pipefd[1], STDOUT_FILENO);
[7] pipe(pipefd);
[8] r = waitpid(child1, &status, 0);
[9] r = waitpid(child2, &status, 0);

Your task is to assign system call IDs, such as “1”, to slots, such as “A”, to achieve several behaviors, including a correct pipeline and several incorrect pipelines. For each question:

You may use each system call ID once, more than once, or not at all.
You may use zero or more system call IDs per slot. Write them in the order they should appear in the code.
You may assume that no signals are delivered to the shell process (so no system call ever returns an EINTR error).
The simple_pipe function should wait for both commands in the pipeline to complete before returning.

Task2: Implement a correct foreground pipeline.
[A]	[B] (child1)	[C]	[D] (child2)	[E]
 	 	 	 	 

Task3: Implement a pipeline so that, given arguments corresponding to “echo foo | wc -c”, the wc process reads “foo” from its standard input but does not exit thereafter. describe in words how this might happen.

[A]	[B] (child1)	[C]	[D] (child2)	[E]
 	 	 	 	 

Task4:

Implement a pipeline so that, given arguments corresponding to “echo foo | wc -c”, “foo” is printed to the shell’s standard output and the wc process prints “0”. (In a correctly implemented pipeline, “wc” would print 4, which is the number of characters in “foo\n”.)  	 	 	 	 

Task5:

Implement a pipeline that appears to work correctly on “echo foo | wc -c”, but always blocks forever if the left-hand command outputs more than 65536 characters

Task6: 

Figure out what code to add at points A, B, and C. in code below :

The function below is intended to traverse a linked list of commands, fork/exec the indicated processes, and hook up the pipes between commands correctly.

struct command {
   command *next; // Next in sequence of commands
   int argc;      // number of arguments
   int ispipe;    // pipe symbol follows this command
   char** argv;   // arguments, terminated by NULL
   pid_t pid;     // pid running this command
};

void do_pipes(command* c) {
   pid_t newpid;
   int havepipe = 0;   // We had a pipe on the previous command
   int lastpipe[2] = {-1, -1};
   int curpipe[2];

   do {
       if (c->ispipe) {
           int r = pipe(curpipe);
           assert(r == 0);
       }

       newpid = fork();
       assert(newpid >= 0);
       if (newpid == 0) {
           if (havepipe) {
               // There was a pipe on the last command; It's stored
               // in lastpipe; I need to hook it up to this process???
               // **** PART A ****
           }
           if (c->ispipe) {
               // The current command is a pipe -- how do I hook it up???
               // **** PART B ****
           }

           execvp(c->argv[0], c->argv);

           fprintf(stderr, "Exec failed\n");
           _exit(1);
       }

       // I bet there is some cleanup I have to do here!?
       // **** PART C ****

       // Set up for the next command
       havepipe = c->ispipe;
       if (c->ispipe) {
           lastpipe[0] = curpipe[0];
           lastpipe[1] = curpipe[1];
       }
       c->pid = newpid;
       c = c->next;
   } while (newpid != -1 && havepipe);
}

 

Questions:

What should go in the Part A space above, if anything?
What should go in the Part B space above, if anything?
What should go in the Part C space above, if anything?
 

 

strace tool:

Strace is one of the most powerful process monitoring, diagnostic, instructional tool of Linux. It also acts as a debugging tool that helps in troubleshooting issues. It is majorly used for the following purposes:

Debugging Programs
Troubleshooting Programs
Intercept System calls by a process
Record system calls by a process
Signals received by a process
Trace running processes
 

Task7 :  Run the following command and interpret the output. 

$strace ls
$strace -c ls    (count the  number of system call)
$strace -e trace=write ls (trace the specific system call)
$strace -T ls  ( time spent on system call )
$strace -o output.txt ls
Task8:
In the remaining questions, we provide strace output for attempts at a shell running a two-process pipeline, echo foo | wc -c. For each question, you are to characterize the shell. This is Shell X1.
58797 pipe([3, 4])                      = 0
58797 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f892b19ea10) = 58798
58797 close(4)                          = 0
58798 close(3)                          = 0
58797 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f892b19ea10) = 58799
58798 dup2(4, 1)                        = 1
58798 close(4)                          = 0
58798 execve("/bin/echo", ["/bin/echo", "foo"], 0x7ffdea26fe98 /* 57 vars */ <... detached ...>
58797 close(3)                          = 0
58797 wait4(58798,  <... unfinished ...>
58799 dup2(3, 0)                        = 0
58799 close(3)                          = 0
58799 execve("/usr/bin/wc", ["/usr/bin/wc", "-c"], 0x7ffdea26fe98 /* 57 vars */ <... detached ...>
58797 <... wait4 resumed> NULL, 0, NULL) = 58798
58797 --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=58798, si_uid=1000, si_status=0, si_utime=0, si_stime=0} ---
58797 wait4(58799, NULL, 0, NULL)       = 58799
58797 --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=58799, si_uid=1000, si_status=0, si_utime=0, si_stime=0} ---
58797 exit_group(0)                     = ?
58797 +++ exited with 0 +++

Questions:
1.What is the process ID of the parent shell?
2.Does Shell X1 wait for the right-hand process, the left-hand process, or both?
3.Does Shell X1 appear to implement two-process pipelines correctly?
 
Task9:
This is Shell X2. It is incorrect.

58969 pipe([3, 4])                      = 0
58969 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f6d566b0a10) = 58970
58970 close(3)                          = 0
58970 dup2(4, 1)                        = 1
58970 close(4)                          = 0
58970 execve("/bin/echo", ["/bin/echo", "foo"], 0x7ffcc0a30220 /* 57 vars */ <... detached ...>
58969 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f6d566b0a10) = 58971
58971 dup2(3, 0)                        = 0
58971 close(3)                          = 0
58971 execve("/usr/bin/wc", ["/usr/bin/wc", "-c"], 0x7ffcc0a30220 /* 57 vars */ <... detached ...>
58969 close(3)                          = 0
58969 close(4)                          = 0
58969 wait4(58970,  <... unfinished ...>
58969 <... wait4 resumed> NULL, 0, NULL) = 58970
58969 --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=58970, si_uid=1000, si_status=0, si_utime=0, si_stime=0} ---
58969 wait4(58971, NULL, 0, NULL)       = ? ERESTARTSYS (To be restarted if SA_RESTART is set)
58969 --- SIGINT {si_signo=SIGINT, si_code=SI_KERNEL} ---
58969 +++ killed by SIGINT +++

Give an strace line, including process ID, that, if added in the right place, would make Shell X2 appear to implement two-process pipelines correctly.


Task10:
This is Shell X3. It is incorrect, though it appears to run correctly on this specific pipeline.

59026 pipe([3, 4])                      = 0
59026 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f0dfa757a10) = 59027
59026 close(4)                          = 0
59027 close(3)                          = 0
59026 wait4(59027,  <... unfinished ...>
59027 dup2(4, 1)                        = 1
59027 close(4)                          = 0
59027 execve("/bin/echo", ["/bin/echo", "foo"], 0x7ffe0672b1b0 /* 57 vars */ <... detached ...>
59026 <... wait4 resumed> NULL, 0, NULL) = 59027
59026 --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=59027, si_uid=1000, si_status=0, si_utime=0, si_stime=0} ---
59026 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f0dfa757a10) = 59028
59026 close(3)                          = 0
59026 wait4(59028,  <... unfinished ...>
59028 dup2(3, 0)                        = 0
59028 close(3)                          = 0
59028 execve("/usr/bin/wc", ["/usr/bin/wc", "-c"], 0x7ffe0672b1b0 /* 57 vars */ <... detached ...>
59026 <... wait4 resumed> NULL, 0, NULL) = 59028
59026 --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=59028, si_uid=1000, si_status=0, si_utime=0, si_stime=0} ---
59026 exit_group(0)                     = ?
59026 +++ exited with 0 +++

Give a two-process Unix pipeline that Shell X3 will not appear to run correctly, and/or briefly describe problem with Shell X3.



Named Pipe
While an unnamed Linux pipe is valid for one process only, a named Linux pipe will ensure that the command is valid the entire time until you shut down the system or delete it. 

mkfifo <named-pipe>     OR
mknod p <named-pipe>
$ mkfifo named-pipe
$ ls > named-pipe
$ cat < named-pipe
