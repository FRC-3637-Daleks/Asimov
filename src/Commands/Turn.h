/*
 * Turn.h
 *
 *  Created on: Apr 4, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_TURN_H_
#define SRC_COMMANDS_TURN_H_


#include "WPILib.h"
#include "Subsystems/Drive.h"

namespace commands
{

/** Command for autonomously driving straight for a certain amount of distance or time
 */
class Turn: public Command
{
public:
	using Drive_t = subsystems::Drive;

public:
	/** Constructs a Turn command
	 * @param drive Drive subsystem
	 * @param velocity -1 to 1 value representing the scale of velocity to drive
	 * @param length amount of revolutions or time to drive for. (Absolute values are compared so signs cant be screwed up \\
	 * This is in meters if \c in_seconds is false, and in seconds if in_seconds is true
	 */
	Turn(Drive_t *drive, double velocity, double length, bool in_seconds = false);

	/** Constructs a Turn command which will not stop until interrupted
	 * @param drive Drive subsystem
	 * @param velocity speed with which to drive
	 */
	Turn(Drive_t *drive, double velocity);

public:
	/// Starts driving forward
	void Initialize() override;

	/// Empty
	void Execute() override;

	/// Returns true when the absolute value of the total distance driven exceeds the length
	bool IsFinished() override;

	/// Stops the drive motors
	void End() override;

	/// Calls End
	void Interrupted() override;

private:
	Drive_t *drive_;
	double velocity_;
	double length_;
	enum {REVOLUTIONS, SECONDS, FOREVER} type_;
};


}





#endif /* SRC_COMMANDS_TURN_H_ */
