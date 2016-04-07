/*
 * ControlIntake.h
 *
 *  Created on: Apr 7, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_CONTROLINTAKE_H_
#define SRC_COMMANDS_CONTROLINTAKE_H_

#include "WPILib.h"

#include "Subsystems/Intake.h"

#include "Utility/ValueStore.h"

namespace commands
{

using namespace dman;

/** Command for manually controlling the velocity of the intake with a joystick
 *
 */
class ControlIntake: public Command
{
public:
	using Intake_t = subsystems::Intake *;
	using Input_t = ValueStore::Value<double>;

public:
	ControlIntake(Intake_t swiss, Input_t input);
	virtual ~ControlIntake() = default;

public:
	/// Sets the mode of the intake to velocity control
	void Initialize() override;

	/// Sets the speed of the intake to the speed from input
	void Execute() override;

	/// Returns false
	bool IsFinished() override;

	/// Stops intake
	void End() override;

	/// Calls End
	void Interrupted() override;

private:
	Intake_t intake_;
	Input_t input_;
};





}






#endif /* SRC_COMMANDS_CONTROLINTAKE_H_ */
