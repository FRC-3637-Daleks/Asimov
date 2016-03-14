/*
 * TankDrive.h
 *
 *  Created on: Feb 26, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_TANKDRIVE_H_
#define SRC_COMMANDS_TANKDRIVE_H_

#include "Subsystems/Drive.h"

#include "WPILib.h"

#include "Utility/ValueStore.h"

namespace commands
{

class TankDrive: public Command
{
public:
	using Drive_t = subsystems::Drive;
	using Input_t = dman::ValueStore::Value<double>;

public:
	/** Constructs a tank drive command which is controlled by \\
	 * the Values pointed to by left and right _control
	 * @left_control likely a joystick input for the left side
	 * @right_control likely a joystick input for the right side
	 * @multiplier a value multiplied by the control values before
	 * @flip If true, the front of the robot is flipped by multiplying the multiplier by -1 \\
	 * and swapping left and right
	 */
	TankDrive(Drive_t *drive, Input_t left_control, Input_t right_control, double mutliplier = .5, bool flip = false);

public:
	/// Checks that the inputs are good to go
	void Initialize() override;

	/// Updates drive to the current left and right inputs
	void Execute() override;

	/// Returns false
	bool IsFinished() override {return false;}

	/// Stops the motors
	void End() override;

	/// Calls End
	void Interrupted() override;

private:
	Drive_t *drive_;
	Input_t left_control_, right_control_;
	double multiplier_;
	bool flip_;
};


}


#endif /* SRC_COMMANDS_TANKDRIVE_H_ */
