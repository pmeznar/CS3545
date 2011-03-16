/*
===========================================================================
File:		files.h
Author: 	Clinton Freeman
Created on: Sep 29, 2010
===========================================================================
*/

#ifndef FILES_H_
#define FILES_H_

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_NAMELENGTH 	128
#define	MAX_FILEPATH	512

int    files_tokenizeStr(char *str, const char *delimiters, char ***tokens);
char * files_readTextFile(char *filename);

#endif /* FILES_H_ */
