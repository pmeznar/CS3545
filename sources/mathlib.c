/*
===========================================================================
File:		mathlib.c
Author: 	Clinton Freeman
Created on: Jan 24, 2011
===========================================================================
*/

#include "headers/mathlib.h"

/*
 * CrossProduct
 */
void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross)
{
	cross[_X] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[_Y] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[_Z] = v1[0]*v2[1] - v1[1]*v2[0];
}

/*
 * VectorNormalize
 * Calculates the unit length vector from an input vector.
 */
vec_t VectorNormalize(const vec3_t in, vec3_t out)
{
	vec_t	length, ilength;

	length = (vec_t)sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]);
	if (length == 0)
	{
		VectorClear(out);
		return 0;
	}

	ilength = (vec_t)(1.0/length);
	out[0] = in[0]*ilength;
	out[1] = in[1]*ilength;
	out[2] = in[2]*ilength;

	return length;
}

/*
 * VectorLength
 * Returns the magnitude of the input vector.
 */
vec_t VectorLength(const vec3_t v)
{
	int		i;
	double	length = 0.0;

	for (i = 0; i < 3; i++)
		length += v[i]*v[i];

	return sqrt(length);
}

/*
 * glmatrix_identity
 * Sets input matrix to the identity.
 */
void glmatrix_identity(float *m)
{
	int i;

	for(i = 0; i < 16; i++)
		if(i%5 == 0)
			m[i] = 1.0;
		else
			m[i] = 0.0;
}
