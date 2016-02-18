/*
 * Swiss.h
 *
 *  Created on: Feb 1, 2016
 *      Author: elija_000
 */



#include "WPILib.h"


#ifndef SRC_SUBSYSTEMS_SWISS_H_
#define SRC_SUBSYSTEMS_SWISS_H_

#include <memory>


namespace subsystems
{

class Swiss: public Subsystem
{
public:
	enum state_t {max=0, door, cheval, min, doordown, end};
	enum mode_t {pos = 0, velocity, vbus};
private:

	static double states[state_t::end];


	struct PIDValues{
		double p;
		double i;
		double d;
	} pPid, vPid;
private:
	static double tickToDegree;
	static double maxVelocity;

	state_t position;
	mode_t mode;
	CANTalon *swisstalon;

public:

	Swiss(int deviceNumber, double P, double I, double D);
	void SetPIDValues(double p1, double p2, double v1, double v2, double b1, double b2);
	void SetVelocity(double v);
	void SetVoltage(double v);
	void SetMode(mode_t m);
	mode_t GetMode();
	void SetState(state_t s);
	void MaxHeight();
	void MinHeight();
	void LowerPort();
	void LiftDoor();
	void LowerCheval();
	state_t GetState();
};


}


#endif /* SRC_SUBSYSTEMS_SWISS_H_ */
