/*
 * math.h
 *
 *  Created on: Jan 19, 2011
 *      Author: Philip
 */

#ifndef MATH_H_
#define MATH_H_

#include <math.h>

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

#define VectorAdd(v1, v2, out) {out[0]=v1[0]+v2[0];out[1]=v1[1]+v2[1];out[2]=v1[2]+v2[2];}
#define VectorSub(v1, v2, out) {out[0]=v1[0]-v2[0];out[1]=v1[1]-v2[1];out[2]=v1[2]-v2[2];}
#define DotProduct(v1, v2, out) {out=v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];}
#define CrossProduct(v1, v2, out) {out[0]=v1[1]*v2[2]-v2[1]*v1[2];out[1]=v1[2]*v2[0]-v1[0]*v2[2];out[2]=v1[0]*v2[1]-v1[1]*v2[0];}
#define VectorScale(v1, num, out) {out[0]=v1[0]*num;out[1]=v1[1]*num;out[2]=v1[2]*num;}
#define VectorCopy(v1, v2) {v2[0]=v1[0];v2[1]=v1[1];v2[2]=v1[2];}
#define VectorClear(v1) {v1[0]=0;v1[1]=0;v1[2]=0;}
#define VectorInverse(v1) {v1[0]=-v1[0];v1[1]=-v1[1];v1[2]=-v1[2];}
#define VectorMagnitude(v1, out) {out=sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);}
#define VectorNormalize(v1) {float temp; VectorMagnitude(v1, temp); VectorScale(v1, 1/temp, v1);}


#endif /* MATH_H_ */
