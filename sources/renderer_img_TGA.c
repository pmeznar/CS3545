/*
===========================================================================
File:		renderer_img_tga.c
Author: 	Clinton Freeman
Created on: Sep 29, 2010
===========================================================================
*/

#include "headers/SDL/SDL_opengl.h"

#include "headers/common.h"
#include "headers/files.h"
#include "headers/mathlib.h"

#include "headers/renderer_materials.h"

#define HEADER_SIZE 18

typedef struct
{
	unsigned char 	idLength, colormapType, imageType;
	unsigned char	colormapSize;
	unsigned short	colormapIndex, colormapLength;
	unsigned short	xOrigin, yOrigin, width, height;
	unsigned char	pixelSize, attributes;
}
tgaHeader_t;

/*
 * Function: renderer_img_loadTGA
 * Description: Loads a TARGA image file, uploads to GL, and returns the
 * texture ID. Only supports 24/32 bit.
 * TODO: Eventually want to look into keeping only a single reference to any
 * given texture file (q3 uses a hashtable?)
 * TODO: Move file checking code elsewhere
 */
void renderer_img_loadTGA(char *name, int *glTexID, int *width, int *height, int *bpp)
{
	int				dataSize, rows, cols, i, j;
	GLuint			type;
	byte			*buf, *imageData, *pixelBuf, red, green, blue, alpha;

	FILE 			*file;
	tgaHeader_t		header;
	struct stat 	st;

	file = fopen(name, "rb");

	if(file == NULL)
	{
		printf("Loading TGA: %s, failed. Null file pointer.\n", name);
		return;
	}

	if(stat(name, &st))
	{
		printf("Loading TGA: %s, failed. Could not determine file size.\n", name);
		return;
	}

	if(st.st_size < HEADER_SIZE)
	{
		printf("Loading TGA: %s, failed. Header too short.\n", name);
		return;
	}

	buf = (byte *)malloc(st.st_size);
	fread(buf, sizeof(byte), st.st_size, file);

	fclose(file);

	memcpy(&header.idLength, 	 	&buf[0],  1);
	memcpy(&header.colormapType, 	&buf[1],  1);
	memcpy(&header.imageType, 		&buf[2],  1);
	memcpy(&header.colormapIndex, 	&buf[3],  2);
	memcpy(&header.colormapLength,  &buf[5],  2);
	memcpy(&header.colormapSize, 	&buf[7],  1);
	memcpy(&header.xOrigin,			&buf[8],  2);
	memcpy(&header.yOrigin,			&buf[10], 2);
	memcpy(&header.width,			&buf[12], 2);
	memcpy(&header.height,			&buf[14], 2);
	memcpy(&header.pixelSize,		&buf[16], 1);
	memcpy(&header.attributes,		&buf[17], 1);

	//Advance past the header
	buf += HEADER_SIZE;

	//I don't care too much about < 24 bits right now, but shouldn't be too hard...
	//TODO: support < 24 bit tga's
	if(header.pixelSize != 24 && header.pixelSize != 32)
	{
		printf("Loading TGA: %s, failed. Only support 24/32 bit images.\n", name);
		return;
	}
	else if(header.pixelSize == 24)
		type = GL_RGB;
	else
		type = GL_RGBA;

	//Determine size of image data chunk in bytes
	dataSize = header.width * header.height * (header.pixelSize / 8);

	//Set up our texture
	*bpp 	 	= header.pixelSize;
	*width  	= header.width;
	*height 	= header.height;

	imageData = (byte *)malloc(dataSize);
	rows	  = *height;
	cols	  = *width;

	if(type == GL_RGB)
	{
		for(i = rows-1; i >= 0; i--)
		{
			pixelBuf = imageData + (i * cols * 3);
			for(j = 0; j < cols; j++)
			{
				blue 	= *buf++;
				green 	= *buf++;
				red		= *buf++;

				*pixelBuf++ = red;
				*pixelBuf++ = green;
				*pixelBuf++ = blue;
			}
		}
	}
	else
	{
		for(i = rows-1; i >= 0; i--)
		{
			pixelBuf = imageData + (i * cols * 4);
			for(j = 0; j < cols; j++)
			{
				blue 	= *buf++;
				green 	= *buf++;
				red		= *buf++;
				alpha	= *buf++;

				*pixelBuf++ = red;
				*pixelBuf++ = green;
				*pixelBuf++ = blue;
				*pixelBuf++ = alpha;
			}
		}
	}

	//Upload the texture to OpenGL
	glGenTextures(1, glTexID);
	glBindTexture(GL_TEXTURE_2D, *glTexID);

	//printf("%d\n", *glTexID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, type, *width, *height,
			0, type, GL_UNSIGNED_BYTE, imageData);

	//Header debugging

	/*
	printf("Attributes: %d\n", 				header.attributes);
	printf("Colormap Index: %d\n", 			header.colormapIndex);
	printf("Colormap Length: %d\n", 		header.colormapLength);
	printf("Colormap Size: %d\n", 			header.colormapSize);
	printf("Colormap Type: %d\n", 			header.colormapType);
	printf("Height: %d\n", 					header.height);
	printf("Identification Length: %d\n",	header.idLength);
	printf("Image Type: %d\n", 				header.imageType);
	printf("Pixel Size: %d\n", 				header.pixelSize);
	printf("Width: %d\n", 					header.width);
	printf("X Origin: %d\n", 				header.xOrigin);
	printf("Y Origin: %d\n", 				header.yOrigin);
	*/
}
/*
 * renderer_img_TGA.c
 *
 *  Created on: Mar 16, 2011
 *      Author: Philip
 */

