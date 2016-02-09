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
{
public:
	enum state_t {max=0, door, cheval, min, end};

private:
	static double states[4];
	state_t position;
	CANTalon *swisstalon;

public:
	Swiss(int deviceNumber, double P, double D);
	void SetState(state_t s);
	void MaxHeight();
	void MinHeight();
	void LiftDoor();
	void LowerCheval();
	state_t GetState();
};


}


#endif /* SRC_SUBSYSTEMS_SWISS_H_ */
