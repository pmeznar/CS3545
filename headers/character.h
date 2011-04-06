/*
 * character.h
 *
 *  Created on: Apr 6, 2011
 *      Author: Philip
 */

#ifndef CHARACTER_H_
#define CHARACTER_H_

typedef struct{
	int weaponNumber;
	double chargeSpeed;
	double turnSpeed;
}weapon;

typedef struct{
	weapon weapon;
}character;





#endif /* CHARACTER_H_ */
