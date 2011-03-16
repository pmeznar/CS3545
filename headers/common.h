/*
===========================================================================
File:		common.h
Author: 	Clinton Freeman
Created on: Sep 29, 2010
===========================================================================
*/

#ifndef COMMON_H_
#define COMMON_H_

//I generally want to avoid header files including other header files, but
//I want just about every module to be able to printf, etc.
#include <stdio.h>
#include <stdlib.h>

#define _X 0
#define _Y 1
#define _Z 2

#define _R 0
#define _G 1
#define _B 2
#define _A 3

typedef unsigned char byte;
typedef enum { efalse, etrue } eboolean;

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768

#endif /* COMMON_H_ */
