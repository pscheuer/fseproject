/**
 * @file pola-i.c
 * @author Scheuer Patrick
 * 
 * @descritpion
 * This file contains the main function of the polash - a minimalistic shell
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 */
 
#include "polash.h"

void main ()
{
  char* cmd = malloc(CMDLEN*sizeof(char));
  int i = 0; // generic counters
  int cont = 1;

	// Intro
	printf("You entered in the minimalistic shell polash\n");
	printf("Press 'exit' to quit.\n");
	
  // set LD_PRELOAD to replace open() and opendir() calls
  setenv("LD_PRELOAD", "./libpola.so", 1);

  // disable interactive mode
  setenv("INTERACTIVE", "0", 1);

  // basic shell
  do
  {
    // reset command string
    memset(cmd, '\0', CMDLEN);
    printf("# ");
    // get command from standard input
    fgets(cmd, CMDLEN, stdin);
    // delete newline at end
    cmd[strlen(cmd)-1] = '\0';

    // set env vars to inform library
    setenv("CMD_LINE", cmd ,1);
    setenv("ALLOWED_WRITES", "-1", 1);

    // special commands
    if (strstr(cmd, "exit") == cmd)
    {
      cont = 0;
      break;
    }

    // execute command
    if (fork() == 0) // child process
    {
      char allowed_writes[2048];
      memset(allowed_writes, '\0', 2048);

      // prepare argument list for execvp
      char** args = malloc(2*sizeof(char*));
      args[0] = strtok(cmd, " ");
      i = 1;
      while ((args[i++] = strtok(NULL, " ")) != NULL)
        args = realloc(args, (i+1)*sizeof(char*));

      // go backwards in argument array
      for (i--; --i>0; )
      {
        // if argument is preceded by a +, that file is explicitly
        // allowed write access
        if (args[i][0] == '+')
        {
          strncpy(allowed_writes, " ", 1);
          strncpy(allowed_writes, args[i]+1, strlen(args[i])-1);
          args[i] = NULL;
        }
      }
      setenv("ALLOWED_WRITES", strlen(allowed_writes) == 0 ? "-1" 
      	:	allowed_writes, 1);

      // execute command
      execvp(args[0], args);

      // executing has failed
      free(args);
      perror("Error executing file: ");
	  }
	  else // wait for command to end
	      wait(NULL);
  }
  while (cont);

  // clean exit
  free(cmd);
}
