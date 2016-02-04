/*
 * Swiss.h
 *
 *  Created on: Feb 1, 2016
 *      Author: elija_000
 */

#include <CANTalon.h>

#ifndef SRC_SUBSYSTEMS_SWISS_H_
#define SRC_SUBSYSTEMS_SWISS_H_

void maxheight();
void minheight();
void liftdoor();
void lowercheval();
void getPosition();

enum state{max=0, door, cheval, min, end};

double state[4];

void getPosition(){
	int selectedSensorPos = cusomMotorDescrip.GetPosition();


}

void maxheight(){



}

void minheight(){




}


void liftdoor(){



}

void lowercheval(){




}
#endif /* SRC_SUBSYSTEMS_SWISS_H_ */
