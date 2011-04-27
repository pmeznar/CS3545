/*
===========================================================================
File:		world_main.c
Author: 	Clinton Freeman
Created on: Oct 5, 2010
===========================================================================
*/

#include "headers/common.h"
#include "headers/mathlib.h"

#include "headers/world_public.h"

static collisionTri_t *collisionList;

static int collisionListPtr;
static int collisionListSize;

/*
 * normalFromTri
 * Generates a normal vector from the given triangle vertices.
 */
void normalFromTri(vec3_t tri[3], vec3_t normal)
{
	vec3_t vec1, vec2, cross;

	VectorSubtract(tri[0], tri[1], vec1);
	VectorSubtract(tri[1], tri[2], vec2);

	CrossProduct(vec1, vec2, cross);
	VectorNormalize(cross, normal);
}

/*
 * world_allocCollisionTris
 */
void world_allocCollisionTris(int numTris)
{
	collisionListSize += numTris;
	collisionList = (collisionTri_t *)realloc(collisionList, sizeof(collisionTri_t) * collisionListSize);
}

/*
 * world_addCollisionTri
 */
void world_addCollisionTri(vec3_t verts[3])
{
	int i;
	vec3_t normal;

	for(i = 0; i < 3; i++)
		VectorCopy(verts[i], collisionList[collisionListPtr].verts[i]);

	normalFromTri(verts, normal);
	VectorCopy(normal, collisionList[collisionListPtr].normal);

	collisionListPtr++;
}

eboolean simpleTest(camera_t camera, collisionTri_t * failTriangle){
	int i, j;
	int xMax, xMin, yMax, yMin, zMax, zMin;
	int tempX, tempY, tempZ, myX, myY, myZ;

	for (i = 0; i < collisionListSize; i++){
		xMax = xMin = collisionList[i].verts[0][0];
		zMax = zMin = collisionList[i].verts[0][1];
		yMax = yMin = collisionList[i].verts[0][2];

		for(j = 0; j < 3; j++){
			tempX = collisionList[i].verts[j][0];
			tempY = collisionList[i].verts[j][2];
			tempZ = collisionList[i].verts[j][1];

			if(xMax < tempX) xMax = tempX;
			if(xMin > tempX) xMin = tempX;
			if(yMax < tempY) yMax = tempY;
			if(yMin > tempY) yMin = tempY;
			if(zMax < tempZ) zMax = tempZ;
			if(zMin > tempZ) zMin = tempZ;
		}

		myX = camera.position[0]; myY = camera.position[1]; myZ = camera.position[2];

		if(myX >= xMin && myX <= xMax &&  myZ >= zMin && myZ <= zMax && myY >= yMin && myY <= yMax){	// && myY >= yMin && myY <= yMax && myZ >= zMin && myZ <= zMax){
			failTriangle->verts[0][0] = xMin; failTriangle->verts[1][0] = xMax;
			failTriangle->verts[0][2] = yMin; failTriangle->verts[1][2] = yMax;
			failTriangle->verts[0][1] = zMin; failTriangle->verts[1][1] = zMax;
			return etrue;
		}
	}

	return efalse;
}
