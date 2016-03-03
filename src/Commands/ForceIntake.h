/*
 * ForceIntake.h
 *
 *  Created on: Mar 3, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_FORCEINTAKE_H_
#define SRC_COMMANDS_FORCEINTAKE_H_
#include "Subsystems/Intake.h"

/**
 * Namespace commands with declaration
 * of ForceIntake command.
 */
namespace commands
{

using Intake = subsystems::Intake;

class ForceIntake : Command
{
// Constructor & destructor:
public:
	ForceIntake(Intake* intake);
	virtual ~ForceIntake() = default;

// Main functions:
public:
	/**
	 * Does nothing but print that the command has started.
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
	 * Stops the Intake's roller after printing that the command is over.
	 */
	void Interrupted() override;

protected:

private:
	Intake *intake_;

};

} // end namespace commands

#endif /* SRC_COMMANDS_FORCEINTAKE_H_ */
