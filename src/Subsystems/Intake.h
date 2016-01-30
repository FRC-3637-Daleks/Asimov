/*
 * Intake.h
 *
 *  Created on: Jan 27, 2016
 *      Author: David
 */

#ifndef SRC_SUBSYSTEMS_INTAKE_H_
#define SRC_SUBSYSTEMS_INTAKE_H_
#include "WPILib.h"

/** Commands namespace: has forward
 * declarations of command classes
 */
namespace commands
{
	class IntakeBall;
} // end namespace commands

/** Subsystems namespace: contains
 * Intake class. Used to carry class
 * definition over to command files.
 */
namespace subsystems
{

class Intake : public Subsystem
{
// Constructor and Destructor:
public:
	Intake();

	virtual ~Intake();

// Functions:
public:
	/** Returns true if the mechanical switch (detector_)is pressed on, and
	 *  false otherwise. Used to determine whether or not to call Stop().
	 */
	bool CheckSwitch() const;

	/** Controls the front rollers to take in and store the boulder. It will
	 * call the SetSpeed function to control the front roller's speed, and
	 * periodically call the CheckSwitch function. If CheckSwitch returns true,
	 * Stop() is called. TakeBall is controlled by the IntakeBall command.
	 *
	 * @param check is used as a safety to determine whether the CheckSwitch
	 * function should be called, just in case it is broken in the field.
	 * check is initially set to true.
	 */
	void TakeBall(bool check = true);

	/** Controls the front rollers in order to horizontally push out the boulder
	 * from its storage. It will call the SetSpeed function to control the speed
	 * of the rollers. PushBall is controlled by the OuttakeBall command.
	 */
	void PushBall();

	/** Sets the speed and direction of the front roller. The argument speed is a
	 * double between -1 and 1. 1 and -1 represent the roller's maximum speed in
	 * two different directions. Negative values for speed indicate that the roller
	 * should move outward (push), while positive values indicate a move inward (intake)
	 *
	 * @param speed the speed of the roller, a double between -1.0 and 1.0
	 */
	void SetSpeed(double speed);

	/** Returns the current speed of the front roller (a value between -1 and 1)
	 */
	double GetSpeed() const;

	/** Calls SetSpeed with argument 0.0 to stop rollers. Called when the End function
	 * of the IntakeBall command is called, or by the TakeBall function.
	 */
	void Stop();

// Commands:
public:
	/** Creates new IntakeBall command, and passes "this" class as an argument
	 */
	commands::IntakeBall * MakeIntakeBall();

protected:

private:

	CANTalon *roller_;
	DigitalInput *detector_;
	double intake_speed_;
	double push_speed_;
};

} // end namespace subsystems

#endif /* SRC_SUBSYSTEMS_INTAKE_H_ */
