/*
===========================================================================
File:		system_files.c
Author: 	Clinton Freeman
Created on: Oct 17, 2010
===========================================================================
*/

#include "headers/common.h"
#include "headers/files.h"

/*
 * Function: files_tokenizeStr
 * Description: Takes an input string and a string containing delimiting characters.
 * It stores an array of new strings that have been broken up by those characters,
 * and returns the number of tokens parsed. This was made to replace using strtok,
 * which is notorious for leading to buggy code (and actually was in my situation).
 * IMPORTANT: The client is responsible for freeing any memory allocated to store
 * tokens!
 */

#define ALLOCGUESS 50000

int files_tokenizeStr(char *str, const char *delimiters, char ***tokens)
{
	unsigned int	curToken, tokenLen, numTokens, spaceAllocated, numDelimiters;

	const char *quote = "\"";

	curToken = tokenLen = numTokens = numDelimiters = 0;

	//Begin by making a guess about how many tokens we will end up having
	spaceAllocated = ALLOCGUESS;
	(*tokens) = (char **)malloc(sizeof(char *) * spaceAllocated);

	while(*str)
	{
		//If we find that we need more space
		if(numTokens >= spaceAllocated-1)
		{
			spaceAllocated *= 2;
			(*tokens) = (char **)realloc((*tokens), (sizeof(char *) * spaceAllocated));
		}

		//Quoted strings must be handled a little bit differently
		//TODO: gross code duplication
		if(*str == '"')
		{
			tokenLen = strcspn(++str, quote);

			//Allocate space, copy characters, null-terminate
			(*tokens)[curToken] = (char *)malloc(sizeof(char) * tokenLen+1);
			strncpy((*tokens)[curToken], str, tokenLen);
			(*tokens)[curToken][tokenLen] = '\0';

			str += tokenLen;
			numDelimiters = strspn(++str, delimiters);
			str += numDelimiters;
		}
		else
		{
			tokenLen = strcspn(str, delimiters);

			//Allocate space, copy characters, null-terminate
			(*tokens)[curToken] = (char *)malloc(sizeof(char) * tokenLen+1);
			strncpy((*tokens)[curToken], str, tokenLen);
			(*tokens)[curToken][tokenLen] = '\0';

			//Advance to the beginning delimiter character, scan the string
			//until we reach a non-delimiting character, skip the cluster
			str += tokenLen;
			numDelimiters = strspn(str, delimiters);
			str += numDelimiters;
		}

		//printf("%s\n", (*tokens)[curToken]);
		numTokens++; curToken++;
	}

	return numTokens;
}

/*
 * file_readTextFile
 */
char * files_readTextFile(char *filename)
{
	FILE	*file;

	int		count		= 0;
	char 	*textData	= NULL;

	if(filename != NULL)
	{
		file = fopen(filename, "rt");

		if(file != NULL)
		{
			fseek(file, 0, SEEK_END);
			count = ftell(file);
			rewind(file);

			if(count > 0)
			{
				textData = (char *)malloc(sizeof(char) * (count+1));
				count 	 = fread(textData, sizeof(char), count, file);

				//Null terminate
				textData[count] = '\0';
			}

			fclose(file);
		}
		else
		{
			printf("Error: unable to open file %s.\n", filename);
			exit(1);
		}
	}

	return textData;
}
