/*
===========================================================================
File:		world_public.h
Author: 	Clinton Freeman
Created on: Dec 12, 2010
===========================================================================
*/

#ifndef WORLD_H_
#define WORLD_H_


//Called by main loop (system_main)
typedef struct
{
	vec3_t	position;
	vec3_t	angles_deg;
	vec3_t	angles_rad;
} camera_t;

typedef struct
{
	vec3_t verts[3];
	vec3_t normal;
}
collisionTri_t;

void world_init();
void world_update();
void world_lerpPositions(float dt);

//Used by model loading (renderer_mesh_ase)
void world_allocCollisionTris(int numTris);
void world_addCollisionTri(vec3_t verts[3]);

eboolean simpleTest(camera_t camera, collisionTri_t * failTriangle);
int tooClose(vec3_t vertex, camera_t camera);

#endif /* WORLD_H_ */
