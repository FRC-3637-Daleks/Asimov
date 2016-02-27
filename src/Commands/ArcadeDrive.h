/*
 * ArcadeDrive.h
 *
 *  Created on: Feb 26, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_ARCADEDRIVE_H_
#define SRC_COMMANDS_ARCADEDRIVE_H_

#include "Subsystems/Drive.h"

#include "WPILib.h"

#include "Utility/ValueStore.h"

namespace commands
{

class ArcadeDrive: public Command
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
	ArcadeDrive(Drive_t *drive, Input_t forward_control, Input_t turn_control,
			double forward_multiplier = .5, double turn_multiplier = .5);

public:
	/// Checks that the inputs are good to go
	void Initialize() override;

	/// Updates drive to the current forward and turn controls
	void Execute() override;

	/// Returns false
	bool IsFinished() override {return false;}

	/// Stops the motors
	void End() override;

	/// Calls End
	void Interrupted() override;

private:
	Drive_t *drive_;
	Input_t forward_control_, turn_control_;
	double forward_multiplier_, turn_multiplier_;
};


}




#endif /* SRC_COMMANDS_ARCADEDRIVE_H_ */
