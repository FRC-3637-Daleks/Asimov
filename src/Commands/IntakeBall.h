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
 * Command namespace with actual
 * implementation of IntakeBall class.
 */
namespace commands
{

using Intake = subsystems::Intake;

class IntakeBall : public Command
{
// Constructors & Destructors:
public:
	IntakeBall(Intake* intake);
	virtual ~IntakeBall() = default;

// Functions:
public:
	/**
	 * Sets up IntakeBall command before it begins execution each time
	 * it is called. Responsible for calling TakeBall function of Intake.
	 */
	void Initialize() override;

	/** Execute is periodically called (every 20 ms), but does nothing.
	 */
	void Execute() override;

	/**
	 * Returns true if IntakeBall no longer needs to be executed, false
	 * if otherwise. Does this by calling Intake's CheckSwitch function.
	 */
	bool IsFinished() override;

	/**
	 * Ends the execution of IntakeBall, calls Intake's Stop function.
	 */
	void End() override;

	/**
	 * Called when another command which requires the same Intake subsystem
	 * is scheduled to run. Calls End function.
	 */
	void Interrupted() override;

protected:

private:
	Intake *intake_;
};

} // end namespace commands

#endif /* SRC_COMMANDS_INTAKEBALL_H_ */
