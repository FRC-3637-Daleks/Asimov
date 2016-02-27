/*
 * SpinUp.h
 *
 *  Created on: Feb 8, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_SPINUP_H_
#define SRC_COMMANDS_SPINUP_H_
#include "Subsystems/Shooter.h"
#include "WPILib.h"

/**
 * Commands namespace with declaration
 * of SpinUp class.
 */
namespace commands
{

using Shooter = subsystems::Shooter;

class SpinUp : public Command
{
using State_t = Shooter::State_t;
// Constructor & destructor:
public:
	SpinUp(Shooter* shooter, double speed = 1.0, double wait_time = 0.25);
	virtual ~SpinUp() = default;

// Main functions:
public:
	/**
	 * Sets up the SpinUp command for exexution each time it's called.
	 * If the current state is not OFF or SPINNINGUP then there is a
	 * problem, and an error message is printed. Otherwise Shooter's
	 * SpinUp function is called and the state is changed to SPINNINGUP
	 * if it isn't already set to that.
	 */
	void Initialize() override;

	/**
	 * Not implemented.
	 */
	void Execute() override {};

	/**
	 * Returns whether the current error is less than or equal to the
	 * allowed error as specified in Shooter.
	 */
	bool IsFinished() override;

	/**
	 * Ends the Execution of SpinUp. Sets the Shooter state to SPUNUP.
	 */
	void End() override;

	/**
	 * Called when another command which requires the same Shooter
	 * subsystem is scheduled. Interrupts the command. Calls the
	 * EmergencyStop functon and sets the Shooter state to OFF.
	 */
	void Interrupted() override;

	/**
	 * Sets the speed of the spinning roller.
	 * @param speed the new roller speed.
	 */
	void SetSpeed(double speed);

	/**
	 * @return speed_ the current top roller speed.
	 */
	double GetSpeed() const;

protected:

private:
	Shooter *shooter_;
	double speed_;
	double wait_time_;
	Timer *timer_;
};

}// end namespce commands

#endif /* SRC_COMMANDS_SPINUP_H_ */
