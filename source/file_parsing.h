/*
 * Utility functions for parsing data from .csv files
 */

#ifndef FILE_PARSING_H
#define FILE_PARSING_H

#include <stdio.h>
#include <string.h>

#define LINE_BUFFER_SIZE 4096
char lineBuffer[LINE_BUFFER_SIZE];

/*
 * Reads a line from a file, and returns a nul-terminated string representation of it,
 * excluding the newline or carriage return and newline at the end.
 * Sets the optional lineLength and hasNextLine arguments
 */
char* readLine(FILE* fp, int* lineLength)
{
	int readChar, nextChar, i;
	if (lineLength != NULL) { *lineLength = 0; }

	//If the file pointer is null, or we've reached the end, return NULL
	if (fp == NULL || feof(fp))
	{
		return NULL;
	}

	//Copy bytes from file to lineBuffer until newline
	for(i = 0; i < LINE_BUFFER_SIZE - 1; ++i)
	{
		readChar = fgetc(fp);
		if (readChar == EOF)
		{
			//EOF, break
			break;
		} else if (readChar == '\n') {
			//Newline -> break
			break;
		} else if (readChar == '\r') {
			//Check if there's a newline following this carriage return
			nextChar = fgetc(fp);
			if (nextChar == '\n') {
				//Newline -> break
				break;
			} else {
				//If not, put it back for later processing
				fputc(nextChar, fp);
			}
		} else {
			//Normal character, add it to buffer
			lineBuffer[i] = (char) readChar;
		}
	}

	//Nul-terminate buffer
	lineBuffer[i] = '\0';

	if (lineLength != NULL) { *lineLength = i; }
	return lineBuffer;
}

/*
 * Divides a string into tokens (substrings) along a specified delimiter character
 * Returns a pointer to the beginning of the substring, or NULL if there are no more tokens
 * If string is NULL, the function will continue parsing from the last non-NULL string input.
 */
char* nextCall = NULL;
char* tokenize( char* string, char delimiter)
{
	char* startFrom;
	char* iterator;
	//Did they pass in a new string?
	if (string == NULL)
	{
		if (nextCall == NULL)
		{
			//If we don't have a new string and the current one is finished, return NULL
			return NULL;
		}
		//If they didn't, continue parsing the previous string
		startFrom = nextCall;
	} else {
		startFrom = string;
	}

	iterator = startFrom;
	//Iterate through the string until we find a nul char or the delimiter
	while( (iterator[0] != '\0') && (iterator[0] != delimiter) )
	{
		//keep moving
		iterator = &(iterator[1]);
	}

	//Check for what we terminated on
	if (iterator[0] == '\0')
	{
		//Found the end of the string
		//No need to set a nul char if there already is one
		//There won't be anything else to tokenize
		nextCall = NULL;
	} else if (iterator[0] == delimiter ) {
		//Found the delimiter
		//Replace this with a nul char
		iterator[0] = '\0';
		//Set nextCall to the character after the delimiter
		nextCall = &(iterator[1]);
	}

	return startFrom;
}

#endif