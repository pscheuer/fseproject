/**
 * @file libpola.c
 * @author Scheuer Patrick
 * 
 * @description
 * This library contains open/-dir function, for polash projet, that reimplement 
 * the standard open function.
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

#include "libpola.h"

extern int errno;

/**
 * Compares two strings
 *
 * @param string1 First string
 * @param string2 Second string
 * @return Boolean value equaling true if strings are the same
 */
 
unsigned int equal_string (char* string1, char* string2)
{
   if (strlen(string1) == strlen(string2) 
   	&& strstr(string1, string2) == string1)
       return 1;
   return 0;
}

/**
 * Isolate < and > from beginning and end of string
 *
 * Arguments given to applications loading this library need to be
 * encapsuled in < and > if write access is allowed.
 * Those brackets need to be removed in order to reference real files.
 *
 * @param string String to strip from
 * @return String without < and >
 */
const char* isol_brace (const char* string)
{
  if (string[0] != '<' || string[strlen(string)-1] != '>')
    return string;

  char* res = malloc((strlen(string)-2)*sizeof(char));
  strncpy(res, string+1, strlen(string)-2);
  return res;
}

/**
 * Records a security breach in the auth.log system log.
 *
 * @param pathname Path of the requested file/directory
 */
void log_sys (const char* pathname)
{
  syslog(LOG_NOTICE | LOG_AUTHPRIV, 
  	"User %s requested non-authorized access to %s\n", getenv("USERNAME"), 
  	pathname);
}

/**
 * Interactive replacement for open
 *
 * This function replaces the open() function.
 * It asks before giving access to the file, first.
 * Possible answers to the access requests are
 *    - Y to accept a request
 *    - N to deny a request
 *
 * @param pathname Path to the file to open
 * @param flags Flags to open the file with
 * @param libc_open Original open() from glibc
 * @return File descriptor of the file to open
 */
int open_i (const char* pathname, int flags, int (*libc_open)(const char *name,
	int flags))
{
  unsigned int ask = 0;
  char a[16];
  char b;
  
  if ((flags & FLAGS) == O_RDONLY)
  {
    printf("Trying to open %s for reading! Allow? (y/n) ", pathname);
    ask = 1;
  }
  else if ((flags & FLAGS) == O_WRONLY)
  {
    printf("Trying to open %s for writing! Allow? (y/n) ", pathname);
    ask = 1;
  }
  else if ((flags & FLAGS) == O_RDWR)
  {
    printf("Trying to open %s for reading and writing! Allow? (y/n) ", pathname);
    ask = 1;
  }
  else 
    printf("Trying to open %s with flags %d!\n", pathname, flags); 
  
  if (ask)
  {        
    memset(a, '\0', 16);       
    fgets(a, 16, stdin);
    sscanf(a, "%c", &b);
    
    if (b != 'y' && b != 'Y')
    {
      log_sys(pathname);
      errno = EACCES;
      return -1;
    }
  }
  return libc_open(pathname, flags);
}

/**
 * Replacement for open implementing the POLA principle
 *
 * This function replaces again the open() function.
 * It implements the following features:
 *    - Read requests are accepted if the file is given in the arguments
 *      (CMD_LINE environment var)
 *    - Write requests are accepted if the file is between brackets (< and >)
 * Refused access rights are logged in the system log.
 *
 * @param pathname Path to the file to open
 * @param flags Flags to open the file with
 * @param libc_open Original open() from glibc
 * @return File descriptor of the file to open
 */
int open_2 (const char* pathname, int flags, int (*libc_open)(const char *name,
	int flags))
{
  char* cmd = getenv("CMD_LINE");
  char* allowed_writes = getenv("ALLOWED_WRITES"); 
      
  if ((flags & FLAGS) == O_RDONLY)
  {
		if (strstr(cmd, pathname) != NULL)
    	return libc_open(pathname, flags);
  }
  else 
  	if ((flags & FLAGS) == O_RDWR || (flags & FLAGS) == O_WRONLY)
			if ((pathname[0] == '<') && (pathname[strlen(pathname)-1] == '>') 
				|| (strstr(allowed_writes, pathname) != NULL))
			{	
				if (pathname[0] == '<')
          {
              const char* file = isol_brace(pathname);
              return libc_open(file, flags);
          }
          return libc_open(pathname, flags);
			}
  log_sys(pathname);
  errno = EACCES;
  return -1;
}

/**
 * Replacement for the original open() calling interactive or non-interactive
 * variants
 *
 * @param pathname Path to the file to open
 * @param flags Flags to open the file with
 * @return File descriptor of the file to open
 */
int open (const char* pathname, int flags, ...)
{
  int (*libc_open)(const char *name, int flags);
  *(void **)(&libc_open) = dlsym(RTLD_NEXT, "open");
  
  char* interactive = getenv("INTERACTIVE");
  if (interactive[0] == '1')
		return open_i(pathname, flags, libc_open);
  else
    return open_2(pathname, flags, libc_open);
}

/**
 * Interactive opendir replacement
 *
 * This function replaces opendir from glibc, implementing a y/n request
 * before opening the directory
 *
 * @param dirname Name of the directory
 * @param libc_opendir Original opendir from glibc
 * @return Pointer to struct dirent of directory
 */
DIR *opendir_i (const char* dirname, DIR *(*libc_opendir)(const char *name))
{
  char a;
  char line[1024];
  printf("Trying to open directory %s! Allow? (Y/n) ", dirname);
  fgets(line, 1024, stdin);
  sscanf(line, "%c", &a);
  if (a == 'Y' || a == 'y')
    return libc_opendir(dirname);
  
  log_sys(dirname);
  errno = EACCES;
  return NULL;
}

/**
 * opendir replacement implementing the POLA principle
 *
 * This function only allows access to the directory if it is explicitly
 * mentioned in the arguments (CMD_LINE environment var)
 *
 * @param dirname Name of the directory
 * @param libc_opendir Original opendir from glibc
 * @return Pointer to struct dirent of directory
 */
DIR *opendir_2 (const char* dirname, DIR *(*libc_opendir)(const char *name))
{
  char* cmd = getenv("CMD_LINE");

  if (strstr(cmd, dirname) != NULL)
		return libc_opendir(dirname);   
  
  log_sys(dirname);
  errno = EACCES;
  return NULL;
}

/**
 * opendir replacement calling the interactive / non-interactive variants
 *
 * @param dirname Name of the directory
 * @return Pointer to struct dirent of directory
 */
DIR *opendir(const char *dirname)
{
  DIR *(*libc_opendir)(const char *name);
  *(void **)(&libc_opendir) = dlsym(RTLD_NEXT, "opendir");
  
  char* interactive = getenv("INTERACTIVE");
  if (interactive[0] == '1')
  	return opendir_i (dirname, libc_opendir);
  else
    return opendir_2 (dirname, libc_opendir);
}
