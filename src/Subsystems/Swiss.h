/*
 * Swiss.h
 *
 *  Created on: Feb 1, 2016
 *      Author: elija_000
 */

#include <CANTalon.h>
#include <Talon.h>x

#ifndef SRC_SUBSYSTEMS_SWISS_H_
#define SRC_SUBSYSTEMS_SWISS_H_

class Swiss{
	void maxheight();
	void minheight();
	void liftdoor();
	void lowercheval();
	void getPosition();
}

enum state{max=0, door, cheval, min, end};

double state[4];

void Swiss::getPosition(){
	customMotorDescrip.SetFeedbackDevice(CANTalon::AnalogPot);
	customMotorDescrip.ConfigPotentiometerTurns(10);


}

void Swiss::maxheight(){



}

void Swiss::minheight(){




}


void Swiss::liftdoor(){



}

void Swiss::lowercheval(){




}
#endif /* SRC_SUBSYSTEMS_SWISS_H_ */
