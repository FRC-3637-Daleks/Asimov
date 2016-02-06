/*
 * Swiss.h
 *
 *  Created on: Feb 1, 2016
 *      Author: elija_000
 */

#include <CANTalon.h>
#include <Talon.h>
#include "WPILib.h"


#ifndef SRC_SUBSYSTEMS_SWISS_H_
#define SRC_SUBSYSTEMS_SWISS_H_

#include <memory>

#define PI 3.1415926535897

namespace subsystems
{

class Swiss: public Subsystem

	enum state{max=0, door, cheval, min, end};
	double state[4];
	int position;


	void maxheight();
	void minheight();
	void liftdoor();
	void lowercheval();
	int getPosition();
}




void getSensorPosition(){


}

int Swiss::getPosition(){
	customMotorDescrip.SetFeedbackDevice(CANTalon::AnalogPot);
	customMotorDescrip.ConfigPotentiometerTurns(10);

	return state;
}

void Swiss::maxheight(){

	//position = state[0];

}

void Swiss::minheight(){

	//position = state[3];

}


void Swiss::liftdoor(){

	//position = state[1];

}

void Swiss::lowercheval(){

	//position= state[2];

}
#endif /* SRC_SUBSYSTEMS_SWISS_H_ */
