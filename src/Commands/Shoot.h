/*
 * Shoot.h
 *
 *  Created on: Feb 13, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_SHOOT_H_
#define SRC_COMMANDS_SHOOT_H_
#include "Subsystems/Shooter.h"
#include "Subsystems/Intake.h"
#include "WPILib.h"

/**
 * Commands namespace with declaration
 * of Shoot command
 */
namespace commands
{

using Shooter = subsystems::Shooter;
using Intake = subsystems::Intake;
using State_t = Shooter::State_t;

class Shoot : public Command
{
// Constructor & destructor:
public:
	Shoot(Intake* intake, Shooter* shooter, double shoot_time);
	virtual ~Shoot() = default;

// Main functions:
public:
	/**
	 * Sets up shoot command. Prints an error message if current state
	 * is not SPUNUP. Otherwise, changes the states of both Intake
	 * and Shooter to SHOOTING and calls Intake's ShootBall function
	 * to spin the front rollers inwards for a certain time.
	 */
	void Initialize() override;

	/**
	 * Not implemented.
	 */
	void Execute() override {};

	/**
	 * Returns true if the current timer value is greater than the time
	 * allowed for the rollers to spin, false if otherwise.
	 */
	bool IsFinished() override;

	/**
	 * Sets the state of Shooter to SPUNUP and the state of Intake to OFF.
	 * Turns off the front rollers.
	 */
	void End() override;

	/**
	 * Calls the end function if another command needs to use the Shooter or
	 * Intake subsystems.
	 */
	void Interrupted() override;

	/**
	 * Sets the time allowed to spend moving the rollers for shooting the ball.
	 * @param shoot_time the new time until End is called.
	 */
	void SetShootTime(double shoot_time);

	/**
	 * @return shoot_time_ the time until End is called. How long the rollers will
	 * spin.
	 */
	double GetShootTime() const;

protected:

private:
	Intake *intake_;
	Shooter *shooter_;
	double shoot_time_;
	Timer *timer_;
};

} // end namespace commands

#endif /* SRC_COMMANDS_SHOOT_H_ */
