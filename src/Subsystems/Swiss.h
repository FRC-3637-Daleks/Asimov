/*
 * Swiss.h
 *
 *  Created on: Feb 1, 2016
 *      Author: elija_000
 */

#ifndef SRC_SUBSYSTEMS_SWISS_H_
#define SRC_SUBSYSTEMS_SWISS_H_

#include "WPILib.h"

#include "WPILib/WPISystem.h"
#include "Utility/ValueStore.h"


#include <memory>
#include <string>


namespace commands
{

class SetSwiss;
class ControlSwissVelocity;
class HoldSwiss;

}

namespace subsystems
{

using namespace dman;

class Swiss: public WPISystem
{
public:
	enum state_t {retract = 0, horizontal, cheval_down, port_down, n_states};
	enum mode_t {pos = CANTalon::ControlMode::kPosition,
				velocity = CANTalon::ControlMode::kSpeed,
				vbus = CANTalon::ControlMode::kPercentVbus};

public:
	static std::string StateToString(state_t state);

private:

	static double states[state_t::n_states];


	struct PIDValues{
		double p;
		double i;
		double d;
		double f;
		unsigned int izone;
	} pPid, vPid;
private:
	static double tickToDegree;
	static double maxVelocity;
	static double allowable_error;

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
	mode_t GetMode() const;
	void SetState(state_t s);
	void RefreshState();

	double GetPos() const;
	double GetDiff() const;
	bool IsCloseNuff() const;
	double get_allowable_error() const {return allowable_error;}
	void SetAllowableError(double err);
	void Hold();

	state_t GetState() const;

public:
	void doRegister() override;
	bool doConfigure() override;
	void initTalon();
	bool is_initialized() const {return swisstalon != nullptr;}

public:
	commands::SetSwiss *MakeSetSwiss(state_t state);
	commands::ControlSwissVelocity *MakeControlSwissVelocity(ValueStore::Value<double> input);
	commands::HoldSwiss *MakeHoldSwiss();
};


}


#endif /* SRC_SUBSYSTEMS_SWISS_H_ */
