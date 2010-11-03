#ifndef H_LIBPOLA
	#define _GNU_SOURCE
	#define H_LIBPOLA
	#define FLAGS (O_RDONLY | O_WRONLY | O_RDWR)
	
	#include <sys/syscall.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <dirent.h>
	#include <errno.h>
	#include <string.h>
	#include <syslog.h>
	#include <dlfcn.h>

	int open (const char* pathname, int flags, ...);
	
	DIR* opendir (const char* dirname);
#endif

unsigned int equal_string (char* string1, char* string2);

const char* isol_brace (const char* string);

void log_sys (const char* pathname);

int open_i (const char* pathname, int flags, int (*libc_open)(const char *name,
	int flags));

int open_2 (const char* pathname, int flags, int (*libc_open)(const char *name,
	int flags));

int open (const char* pathname, int flags, ...);

DIR *opendir_i (const char* dirname, DIR *(*libc_opendir)(const char *name));

DIR *opendir_2 (const char* dirname, DIR *(*libc_opendir)(const char *name));

DIR *opendir(const char *dirname);
