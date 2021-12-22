#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>

/*
Author: Kyle White
Class: CS360 Algorithms
Date: 10/29/2021
*/

/*
The pwd function returns a char array containing the current working
directory, obtained by using the function getcwd.
*/
char *pwd() {
  static char cwd[256];
  // Use built in getcwd function to update cwd, print error if returns NULL.
  if(getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("Error processing getcwd command:");
  }
  return cwd;
}

/*
execExternal is a void function that accepts a char** as an argument
which formats the char** into an argv array for use with the
execvp external function call.
*/
void execExternal(char *command) {
  // Maximum number of arguments
  int maxArgs = 64;
  // Current number of arguments
  int argc = 0;
  // Create a pointer to a char array containing the maximum number of args.
  char *argv[maxArgs];
  // Create a token for keeping track of tokenized input string
  char *token = strtok(command, " ");
  // While the token is not null and argc is less than maxArgs-1
  while (token && argc < maxArgs-1)
  {
    // Increment argc++ and place the token in its place in argv.
    argv[argc++] = token;
    // Advance the token to the next one in the input command string.
    token = strtok(NULL, " ");
  }
  // Set the last element of the array to null for execvp.
  argv[argc] = NULL;

  // Forking a child
  pid_t child_pid = fork();
  if (child_pid == 0)
  {
    // In the child process, if execvp returns -1, print returned error.
    if(execvp(argv[0], argv) < 0)
    {
      perror("Could not execute command.");
    }
    // Exit child process.
    exit(0);
  } else if (child_pid < 0) {
    perror("Error: failed to create process.");
  }
  else
  {
    // Wait for child to terminate.
    wait(&child_pid);
    return;
  }
}

/*
The userInput function returns a pointer to a char array gathered
from the console using fgets.
*/
char* userInput() {
  // Create buffer size.
  int buffer = 256;
  // Allocate memory for char array using buffer size.
  char *input = malloc(buffer);
  // Use fgets to accept user console input.
  if (fgets(input, buffer, stdin) != NULL) {
    // Strip newline character from the end of the input line.
    input[strcspn(input, "\n")] = 0;
    return input;
  } else {
    return NULL;
  }

}

/*
The main function of this shell program selects one of three built in functions
based on user input, or directs the user's input commands to an external call
function.
*/
int main() {
  // Char* for storing the current working directory.
  char *wd;
  while(1) {
    // Update current working directory using pwd function.
    wd = pwd();
    // Print current working directory and $mysh> format.
    printf("%s$mysh> ", wd);
    // Create int for buffer size 256 from user input.
    int buffer = 256;
    // Gather user input using userInput function.
    char *input = userInput();
    // Create a copy of input (that will not be parsed for tokens)
    char nonParsed[buffer];
    strncpy(nonParsed, input, buffer);
    // Create char *args for storing input tokens.
    char *args;
    // Use strtok to populate args with the first parsed token.
    args = strtok(input, " ");
    // While args is not null (meaning we have not reached the end of the string)
    while (args != NULL)
    {
      // If args == "pwd"
      if (!strcmp(args, "pwd"))
      {
        // Update current working directory
        wd = pwd();
        // Print current working directory
        printf("%s\n", wd);
        // Advance the args pointer (in this case, it should grab
        // NULL which breaks the loop)
        args = strtok(NULL, " ");
      }
      // If args == "cd"
      else if (!strcmp(args, "cd"))
      {
        // Advance the args pointer which will grab the next token
        args = strtok(NULL, " ");
        // If chdir returns something other than 0, print returned error.
        if(chdir(args) != 0) {
          perror("Error");
        }
        // Advance the args pointer which will return NULL and break loop.
        args = strtok(NULL, " ");
      }
      // If args == "exit", exit the program and return 0.
      else if (!strcmp(args, "exit"))
      {
        exit(0);
      }
      // If args doesn't match the three built in functions, then it must be
      // external. Send it to execExternal function for handling.
      else
      {
        // Send non-parsed user input into execExternal for handling.
        execExternal(nonParsed);
        // Break the loop and return to main while loop.
        break;
      }
      // Advance the pointer to clear the input string.
      args = strtok(NULL, " ");
    }
  }
  return 0;
}
