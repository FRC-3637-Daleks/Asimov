/*
 * ControlSwissVelocity.h
 *
 *  Created on: Mar 3, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_CONTROLSWISSVELOCITY_H_
#define SRC_COMMANDS_CONTROLSWISSVELOCITY_H_

#include "WPILib.h"

#include "Subsystems/Swiss.h"

#include "Utility/ValueStore.h"

namespace commands
{

using namespace dman;

/** Command for manually controlling the velocity of the swiss with a joystick
 *
 */
class ControlSwissVelocity: public Command
{
public:
	using Swiss_t = subsystems::Swiss *;
	using Input_t = ValueStore::Value<double>;

public:
	ControlSwissVelocity(Swiss_t swiss, Input_t input);
	virtual ~ControlSwissVelocity() = default;

public:
	/// Sets the mode of the swiss to voltage control
	void Initialize() override;

	/// Sets the speed of the swiss to the speed from input
	void Execute() override;

	/// Returns false
	bool IsFinished() override;

	/// Holds swiss wherever it was
	void End() override;

	/// Calls End
	void Interrupted() override;

private:
	Swiss_t swiss_;
	Input_t input_;
};





}



#endif /* SRC_COMMANDS_CONTROLSWISSVELOCITY_H_ */
