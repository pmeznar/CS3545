/*
===========================================================================
File:		renderer_materials.c
Author: 	Clinton Freeman
Created on: Oct 8, 2010
Notes:		Please remember that this is, for the moment, basically
			just ripped directly out of my project, and so you will
			need to do some re-working to fit it into your project.
===========================================================================
*/

#include "headers/SDL/SDL_opengl.h"

#include "headers/common.h"
#include "headers/files.h"
#include "headers/mathlib.h"

#include "headers/renderer_materials.h"

typedef struct
{
	int		glTexID;
	char	name[128];
	vec3_t	ambient, diffuse, specular;
	float	shine, shineStrength, transparency;
	int		width, height, bpp;
}
material_t;

static material_t materialList[MAX_TEXTURES];
static int stackPtr = 0;

/*
 * renderer_img_createMaterial
 */
int renderer_img_createMaterial(char *name, vec3_t ambient, vec3_t diffuse, vec3_t specular,
		float shine, float shineStrength, float transparency)
{
	material_t *currentMat = &materialList[stackPtr];

	currentMat->shine 			= shine;
	currentMat->shineStrength 	= shineStrength;
	currentMat->transparency 	= transparency;

	strcpy(currentMat->name, name);

	VectorCopy(ambient,  currentMat->ambient);
	VectorCopy(diffuse,  currentMat->diffuse);
	VectorCopy(specular, currentMat->specular);

	renderer_img_loadTGA(name, &(currentMat->glTexID),
			&(currentMat->width), &(currentMat->height), &(currentMat->bpp));

	return stackPtr++;
}

int renderer_img_getMatGLID  (int i) { return materialList[i].glTexID; }
int renderer_img_getMatWidth (int i) { return materialList[i].width;   }
int renderer_img_getMatHeight(int i) { return materialList[i].height;  }
int renderer_img_getMatBpp   (int i) { return materialList[i].bpp;     }/*
 * renderer_materials.c
 *
 *  Created on: Mar 16, 2011
 *      Author: Philip
 */

