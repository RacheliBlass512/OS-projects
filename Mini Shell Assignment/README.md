# Mini Shell Assignment
## goal
The goal of this assignment is to gain experience with process management, pipes, signals, and the
relevant system calls. To do this, I implemented a simple shell program. I implemented
the function that receives a shell command and performs it. I was provided with a skeleton shell program
(shell.c), which reads lines from the user, parses them into commands, and invokes my function.

## The shell skeleton
The skeleton we provide executes an infinite loop. Each iteration does the following:
1. Reads a string containing a shell command from standard input. If Ctrl-D is pressed, the skeleton
exits.
2. Parses the command string into an array of words. A word is a non-empty sequence of nonwhitespace characters, where whitespace means space, tab (\t), or newline (\n). The end of the
array is designated with a NULL entry.
3. Invokes my function, passing it the parsed command line array. The skeleton detects and ignores
empty lines (i.e., won’t invoke my function on an empty command).

##  Shell functionality
The shell will support the following operations, which are described in more detail below:
1. **Executing commands**. The user enters a command, i.e., a program and its arguments, such as
sleep 10. The shell executes the command and waits until it completes before accepting another
command.
2. **Executing commands in the background**. The user enters a command followed by `&`, for
example: `sleep 10 &`. The shell executes the command but does not wait for its completion
before accepting another command.
3. **Single piping**. The user enters two commands separated by a pipe symbol (`|`), for example:
`cat foo.txt | grep bar`. The shell executes both commands, piping the output of the first
command to the input of the second command. The shell waits until both commands complete
before accepting another command.

The shell doesn’t need to support built-in commands such as cd and exit. It only support
execution of program binaries as described above.

## ASSUMPTIONS
1. If a command line contains the | symbol, then (1) it appears only once and (2) it is correctly
placed, i.e., at least one word appears before and after it, and it is separated by whitespace from
them.
2. If a command line contains the & symbol, then it appears last on the command line and is
separated by whitespace. In other words, it is the last word of the command line.
3. A command line will not contain both | and & symbols. In other words, pipes and background
execution are not combined.
4. Command lines will not contain quotation marks or apostrophes. (In normal shells, these are
used to support arguments that contain whitespace. Your shell doesn’t need to support this
feature.)
## Assignment description
I was neede to implement the following functions in a file named myshell.c: `prepare()`, `process_arglist()`,
and `finalize()`. The following details the specifications of these functions.
## `int prepare(void)`
The skeleton calls this function before the first invocation of `process_arglist()`. This function
returns 0 on success; any other return value indicates an error.
You can use this function for any initialization and setup that you think are necessary for your
`process_arglist()`.

## `int process_arglist(int count, char **arglist)`
**Input** This function receives an array `arglist` with count non-NULL words. This array contains
the parsed command line. The last entry in the array, `arglist[count]`, is NULL. (So overall the
array size is `count+1`.)

#### Behavior
1. Commands specified in the `arglist` should be executed as a child process using `fork()` and
`execvp()` (not `execv()`). Notice that if the `arglist` array contains a shell symbol (`&` or `|`), the
arglist should not be passed to `execvp()` as-is (more details below).
2. Executing commands in the background:
    * If the last non-NULL word in `arglist` is "`&`" (a single ampersand), run the child process
    in the background. The parent should not wait for the child process to finish, but instead
    continue executing commands.
    * Do not pass the `&` argument to `execvp()`.
    * Assume background processes don’t read input (stdin).

3. Piping:
* If `arglist` contains the word "`|`" (a single pipe symbol), run two child processes, with the
output (stdout) of the first process (executing the command that appears before the pipe)
piped to the input (stdin) of the second process (executing the command that appears after
the pipe).
* To pipe the child processes input and output, use the `pipe()` and `dup2()` system calls.
* Use the same array for all `execvp()` calls by referencing items in `arglist`. There’s no need
to allocate a new array and duplicate parts of the original array.
4. Handling of SIGINT:

* **Background on SIGINT** (this bullet contains things you should know, not things you
need to implement in the assignment): When the user presses Ctrl-C, a SIGINT signal is sent
(by the OS) to the shell and all its child processes. The SIGINT signal can also be sent to a
specific process using the `kill()` system call. 
* After `prepare()` finishes, the parent (shell) should not terminate upon SIGINT.
* Foreground child processes (regular commands or parts of a pipe) should terminate upon
SIGINT.
* Background child processes should not terminate upon SIGINT.

**Output**
1. The `process_arglist()` function should not return until every foreground child process it
created exits.
2. In the original (shell/parent) process, `process_arglist()` should return 1 if no error occurs.
(This makes sure the shell continues processing user commands.) If an error occurs in the parent
process, it should print an error message and return 0.

## ```int finalize(void)```
The skeleton calls this function before exiting. This function returns 0 on success; any other return
value indicates an error.

You can use this function for any cleanups related to `process_arglist()` that you think are
necessary. If you don’t need any cleanups, just have this function return immediately; but you
must provide it for the skeleton to compile. Note that cleaning up the `arglist` array is not your
responsibility. It is taken care of by the skeleton code.

## compile:
```cmd
gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 shell.c myshell.c
```