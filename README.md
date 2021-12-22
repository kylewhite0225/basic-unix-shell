# Basic Unix Shell

This is a basic unix shell with three built in commands: cd (change directory), pwd (print working directory), and exit.
External commands are handled by a function called execExternal that tokenizes the input arguments, creates a child process using fork(),
passes the arguments to the child process using execvp, and then waits for the child to terminate.
