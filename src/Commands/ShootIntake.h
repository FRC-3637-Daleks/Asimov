/*
 * ShootIntake.h
 *
 *  Created on: Feb 22, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_SHOOTINTAKE_H_
#define SRC_COMMANDS_SHOOTINTAKE_H_
#include "Subsystems/Shooter.h"
#include "Subsystems/Intake.h"
#include "WPILib.h"

/**
 * Commands namespace with declaration
 * of ShootIntake command.
 */
namespace commands
{

using Shooter = subsystems::Shooter;
using Intake = subsystems::Intake;

class ShootIntake : public Command
{
using State_t = Shooter::State_t;

// Constructor & destructor:
public:
	ShootIntake(Intake* intake, Shooter* shooter, double shoot_time = 0.25, double timeout = 1.0);
	virtual ~ShootIntake() = default;

// Main functions:
public:
	/**
	 * Sets up the ShootIntake command. Prints an error message if the
	 * current state is not SPUNUP. Otherwise, starts the timer object,
	 * changes the state of intake and shooter to SHOOTING and calls the
	 * ShootBall function of Intake.
	 */
	void Initialize() override;

	/**
	 * Execute is called every 20 ms to check whether the mechanical switch
	 * of Intake is disengaged. If it is, then the timer will start. A boolean
	 * flag is also included to ensure that the timer doesn't reset every 20 ms
	 * after starting.
	 */
	void Execute() override;

	/**
	 * Responsible for state changes and calling the Stop function of Intake.
	 */
	void End() override;

	/**
	 * @return true if either the command is timed out or if the value of the timer
	 * object is greater than or equal to shoot_time_.
	 */
	bool IsFinished() override;

	/**
	 * End the command if the current state of Intake is SHOOTING, calls End.
	 * This is to prevent intake from ending in the wrong state.
	 */
	void Interrupted() override;

	/**
	 * Sets the time allowed to spend moving the rollers for shooting the ball.
	 * @param shoot_time the new time until End is called.
	 */
	void SetShootTime(double shoot_time);

	/**
	 * @return shoot_time_ how long the roller's will spin before End() is called.
	 */
	double GetShootTime() const;

	/**
	 * Sets the overall shooting timout for the command (calls SetTimeout).
	 * @param time_out the new timeout.
	 */
	void SetOverallTimeout(double time_out);

	/**
	 * @return time_out_ the current ocerall timeout for ending command execution.
	 */
	double GetOverallTimeout() const;

protected:

private:
	subsystems::Intake *intake_;
	Shooter *shooter_;
	bool check_switch_;
	double shoot_time_;
	double timeout_;
	Timer *timer_;
};

} // end namespace commands

#endif /* SRC_COMMANDS_SHOOTINTAKE_H_ */
