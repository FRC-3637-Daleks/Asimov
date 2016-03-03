/*
 * ForcePush.h
 *
 *  Created on: Mar 3, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_FORCEPUSH_H_
#define SRC_COMMANDS_FORCEPUSH_H_
#include "Subsystems/Intake.h"

/**
 * Namespace commands with declaration
 * of ForcePush command.
 */
namespace commands
{

using Intake = subsystems::Intake;

class ForcePush : Command
{
// Constructor & destructor:
public:
	ForcePush(Intake *intake);
	virtual ~ForcePush() = default;

// Main functions:
public:
	/**
	 * Does nothing but print that the command started.
	 */
	void Initialize() override;

	/**
	 * Not implemented.
	 */
	void Execute() override {};

	/**
	 * Returns false. Command only ends if interrupted.
	 */
	bool IsFinished() override;

	/**
	 * Not implemented.
	 */
	void End() override {};

	/**
	 * Prints that the command is over, and stops intake roller.
	 */
	void Interrupted() override;

protected:

private:
	Intake *intake_;
};


} // end namespace commands

#endif /* SRC_COMMANDS_FORCEPUSH_H_ */
