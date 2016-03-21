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
#include "Commands/ShootIntake.h"
#include "Commands/SpinDown.h"
#include "WPILib.h"

/**
 * Commands namespace with declaration
 * of Shoot command group
 */
namespace commands
{

using Shooter = subsystems::Shooter;
using Intake = subsystems::Intake;

class Shoot : public CommandGroup
{
using State_t = Shooter::State_t;

// Constructor & destructor:
public:
	Shoot(Intake* intake, Shooter* shooter, double wait_time = 0.5, double shoot_time = 0.25, double timeout = 1.0);
	virtual ~Shoot() = default;

// Main functions:
public:

	/**
	 * So far does nothing but print out that the command group has started,
	 * and make sure that shooter is in the right state.
	 */
	void Initialize() override;

	/**
	 * Sets shoot_time_ and the shoot time of ShootIntake to a new value.
	 * @param shoot_time the new shoot time value of ShootIntake.
	 */
	void SetShootTime(double shoot_time);

	/**
	 * @return shoot_time_ the current shoot time of th ShootIntake command.
	 */
	double GetShootTime() const;

	/**
	 * Sets timeout_ and the timeout of ShootIntake to a new value.
	 * @param timeout the new timeout value of ShootIntake.
	 */
	void SetTimeOut(double timeout);

	/**
	 * @return timeout_ the current timeout of the ShootIntake command.
	 */
	double GetTimeOut() const;

	/**
	 * Sets wait_time_ for SpinDown to a new cvalue
	 * @param wait_time the new timeout for SpinDown
	 */
	void SetWaitTime(double wait_time);

	/**
	 * @return wait_time_ the timeout for SpinDown
	 */
	double GetWaitTime() const;

protected:

private:
	subsystems::Intake *intake_;
	Shooter *shooter_;

	double shoot_time_;
	double timeout_;
	double wait_time_;

	ShootIntake *shoot_intake_;
	SpinDown *spin_down_;
};

} // end namespace commands

#endif /* SRC_COMMANDS_SHOOT_H_ */
