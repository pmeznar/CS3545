/*
===========================================================================
File:		renderer_materials.h
Author: 	Clinton Freeman
Created on: Feb 19, 2011
===========================================================================
*/

#ifndef RENDERER_MATERIALS_H_
#define RENDERER_MATERIALS_H_

#define MAX_TEXTURES 512

int renderer_img_createMaterial(char *name, vec3_t ambient, vec3_t diffuse, vec3_t specular,
		float shine, float shineStrength, float transparency);

int renderer_img_getMatGLID(int i);
int renderer_img_getMatWidth(int i);
int renderer_img_getMatHeight(int i);
int renderer_img_getMatBpp(int i);

void renderer_img_loadTGA(char *name, int *glTexID, int *width, int *height, int *bpp);

#endif /* RENDERER_MATERIALS_H_ */


