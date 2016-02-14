/*
 * SpinDown.h
 *
 *  Created on: Feb 10, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_SPINDOWN_H_
#define SRC_COMMANDS_SPINDOWN_H_
#include "Subsystems/Shooter.h"

/**
 * Commands namespace with declaration
 * of SpinDown class.
 */
namespace commands
{

using Shooter = subsystems::Shooter;
using State_t = Shooter::State_t;

class SpinDown : public Command
{
// Constructors & destructor:
public:
	SpinDown(Shooter* shooter);
	virtual ~SpinDown() = default;

// Main functions:
public:
	/**
	 * Sets up the SpinDown command for execution each time it's
	 * called. If the current state is not SPUNUP, an error message
	 * is printed. Otherwise the SpinDown function is called and
	 * the state of Shooter is set to SPINNINGDOWN.
	 *
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
	 * Sets the state of the Shooter subsystem to OFF.
	 */
	void End() override;

	/**
	 * Not implemented.
	 */
	void Interrupted() override {};

protected:

private:
	Shooter *shooter_;
};

}// end namespace commands

#endif /* SRC_COMMANDS_SPINDOWN_H_ */
