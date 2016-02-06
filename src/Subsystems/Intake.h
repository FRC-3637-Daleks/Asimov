/*
 * Intake.h
 *
 *  Created on: Jan 27, 2016
 *      Author: David
 */

#ifndef SRC_SUBSYSTEMS_INTAKE_H_
#define SRC_SUBSYSTEMS_INTAKE_H_
#include "WPILib.h"

/**
 * Commands namespace: has forward
 * declarations of command classes.
 */
namespace commands
{
	class IntakeBall;
} // end namespace commands

/**
 * Subsystems namespace for carying
 * Intake class definition over to
 * command files.
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
	/**
	 * Returns true if the mechanical switch (detector_) is
	 * pressed on, and false otherwise. Used to determine
	 * whether or not to call Stop().
	 */
	bool CheckSwitch() const;

	/**
	 * Controls the front rollers to take in and store the boulder.
	 * Calls SetSpeed to conrol the roller's speed, and CheckSwitch
	 * to determine if the rollers should be stopped. If CheckSwitch
	 * and variable check are both true, Stop is called.
	 * NOTE: Relies on the IntakeBall command
	 *
	 * @param check is used as a safety measure determine if the
	 * CheckSwitch function should be called just in case the switch
	 * itself malfunctions in the field. Check is defaulted to true.
	 */
	void TakeBall(bool check = true);

	/**
	 * Controls the front rollers in order to horizontally push the
	 * boulder from its storage. It will call the SetSpeed function to
	 * control the speed of the rollers.
	 * NOTE: PushBall is controlled by the OuttakeBall command.
	 */
	void PushBall();

	/**
	 * Sets the speed and direction of the front roller. Values of -1 or
	 * 1 for speed represent the roller's maximum possible speed in two
	 * dirtectioms. Negative values indicate that the roller should move
	 * outward (push ball) while positive values indicate the opposite.
	 *
	 * @param speed the speed of the roller, a double between -1.0 and 1.0
	 */
	void SetSpeed(double speed);

	/** Returns the front roller's current speed (a value between -1 and 1)
	 */
	double GetSpeed() const;

	/**
	 * Calls SetSpeed with argument 0.0 to stop rollers. Only called by either
	 * TakeBall or when the IntakeBall command's End function is invoked.
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
