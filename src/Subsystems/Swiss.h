/*
 * Swiss.h
 *
 *  Created on: Feb 1, 2016
 *      Author: elija_000
 */

#ifndef SRC_SUBSYSTEMS_SWISS_H_
#define SRC_SUBSYSTEMS_SWISS_H_


#include "WPILib.h"



#include <memory>


namespace subsystems
{

class Swiss: public Subsystem
{
public:
	enum state_t {retract, horizontal, cheval_down, port_down, n_states};
	enum mode_t {pos = CANTalon::ControlMode::kPosition,
				velocity = CANTalon::ControlMode::kSpeed,
				vbus = CANTalon::ControlMode::kPercentVbus};
private:

	static double states[state_t::n_states];


	struct PIDValues{
		double p;
		double i;
		double d;
		double f;
		double izone;
	} pPid, vPid;
private:
	static double tickToDegree;
	static double maxVelocity;

	state_t current;
	state_t position;
	CANTalon *swisstalon;

public:

	Swiss();
	void SetPIDValues(double p1, double p2, double v1, double v2, double b1, double b2);

	/** If change mode is true it will change the mode of the system to velocity.
	 * Otherwise it will just set the talon to v
	 *
	 */
	void SetVelocity(double v, bool changeMode = true);
	void SetVoltage(double v);
	void SetMode(mode_t m);
	mode_t GetMode();
	void SetState(state_t s);
	void RefreshState();

	double GetPos();
	double GetDiff();
	bool IsCloseNuff();
	void Hold();

	state_t GetState();
};


}


#endif /* SRC_SUBSYSTEMS_SWISS_H_ */
