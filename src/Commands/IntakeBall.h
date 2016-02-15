/*
 * IntakeBall.h
 *
 *  Created on: Jan 30, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_INTAKEBALL_H_
#define SRC_COMMANDS_INTAKEBALL_H_
#include "Subsystems/Intake.h"

/**
 * Command namespace with declaration
 * of IntakeBall class.
 */
namespace commands
{

using Intake = subsystems::Intake;

class IntakeBall : public Command
{
using State_t = Intake::State_t;
// Constructor & destructor:
public:
	IntakeBall(Intake* intake);
	virtual ~IntakeBall() = default;

// Functions:
public:
	/**
	 * Sets up IntakeBall command before it begins execution each time
	 * it is called. If the current state is not OFF, then there is a
	 * problem, and an error message is printed. Otherwise, the TakeBall
	 * function of Intake is called and the state is set to TAKING.
	 */
	void Initialize() override;

	/**
	 * Execute is periodically called (every 20 ms), but not implemented.
	 */
	void Execute() override {};

	/**
	 * Returns true if IntakeBall no longer needs to be executed, false
	 * if otherwise. Does this by calling Intake's CheckSwitch function.
	 */
	bool IsFinished() override;

	/**
	 * Ends the execution of IntakeBall. Calls the Stop function of Intake
	 * and sets the new state to HOLDING
	 */
	void End() override;

	/**
	 * Called when another command which requires the same Intake subsystem
	 * is scheduled to run. Interrupts the command. Calls the Stop function
	 * and sets the Intake state to OFF.
	 */
	void Interrupted() override;

protected:

private:
	Intake *intake_;
};

} // end namespace commands

#endif /* SRC_COMMANDS_INTAKEBALL_H_ */
