/*
 * ForceShoot.h
 *
 *  Created on: Mar 3, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_FORCESHOOT_H_
#define SRC_COMMANDS_FORCESHOOT_H_
#include "Subsystems/Intake.h"

/**
 * Commands namespace with declaration
 * of ForceShoot command.
 */
namespace commands
{

using Intake = subsystems::Intake;

class ForceShoot : public Command
{
// Constructor & destructor:
public:
	ForceShoot(Intake *intake);
	virtual ~ForceShoot() = default;

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
	 * Returns false. Command does not end unless Interrupted.
	 */
	bool IsFinished() override;

	/**
	 * Not implemented.
	 */
	void End() override {};

	/**
	 * Prints that the command is over and stops intake rollers.
	 */
	void Interrupted() override;

protected:

private:
	Intake *intake_;
};

} // end namespace commands

#endif /* SRC_COMMANDS_FORCESHOOT_H_ */
