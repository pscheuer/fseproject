/**
 * @file pola-i.c
 * @author Scheuer Patrick
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

#include "pola-i.h"

int main (int argc, char** argv)
{
	int i;
	char* libpath = getcwd(NULL, 0);
	
	libpath = realloc(libpath, (11+strlen(libpath))*sizeof(char));
	strncat(libpath, "/libpola.so", 11);
	setenv ("LD_PRELOAD", libpath, 1);
	free(libpath); 
	
	setenv("INTERACTIVE", "1", 1);
	
	if (argc < 2)
	{
	  printf("Usage: %s <command> [arg1] [arg2] ...\n", argv[0]);
	  exit(1);
	}
	
	for (i=1; i<argc; i++)
	  argv[i-1] = argv[i];
	argv[i-1] = NULL;
	
	execvp(argv[0], argv);
	perror("Error executing file: ");
	
	return 0;
}