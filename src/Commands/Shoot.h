/*
 * Shoot.h
 *
 *  Created on: Feb 13, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_SHOOT_H_
#define SRC_COMMANDS_SHOOT_H_
#include "Subsystems/Shooter.h"

/**
 * Commands namespace with declaration
 * of Shoot command
 */
namespace commands
{

using Shooter = subsystems::Shooter;
using State_t = Shooter::State_t;

class Shoot : public Command
{
// Constructor & destructor:
public:
	Shoot(Intake* intake, Shooter* shooter);
	virtual ~Shoot() = default;

// Main functions:
public:
	/**
	 * Sets up shoot command before executiion each time it is called.
	 *
	 */
	void Initialize() override;

	/**
	 *
	 */
	void Execute() override;

	/**
	 *
	 */
	bool IsFinished() override;

	/**
	 *
	 */
	void End() override;

	/**
	 *
	 */
	void Interrupted() override;

protected:

private:
	Intake *intake_;
	Shooter *shooter_;
};

} // end namespace commands

#endif /* SRC_COMMANDS_SHOOT_H_ */
