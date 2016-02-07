/*
 * PushBall.h
 *
 *  Created on: Feb 6, 2016
 *      Author: David
 */

#ifndef SRC_COMMANDS_PUSHBALL_H_
#define SRC_COMMANDS_PUSHBALL_H_
#include "Subsystems/Intake.h"

namespace commands
{

using Intake = subsystems::Intake;

class PushBall: public CommandGroup
{
// Construcotrs & Destructors:
public:
	PushBall(Intake* intake);
	virtual ~PushBall() = default;

// Functions:
public:
	/**
	 * Initializes the PushBall command group before it begins execution.
	 * Responsable for creating Push and Stopper commands, passing the
	 * Intake parameter to them. Sequentially adds the Push Command, a
	 * WaitCommand for delaying the stopping of the rollers, and the
	 * Stopper command.
	 */
	void Initialize() override {};

	/**
	 * Execute is periodically called (every 20 ms) but does nothing.
	 */
	void Execute() override {};

	/**
	 * Returns true if PushBall no longer needs to be executed, false
	 *if otherwise.
	 */
	bool IsFinished() override {};

	/**
	 * Does nothing as stopping the rollers is handled by Stopper
	 */
	void End() override {};

	/**
	 * Does nothing.
	 */
	void Interrupted() override {};

	/**
	 * Sets timeout_ to a specified timeout value, for the purpose of waiting
	 * to stop turning the rollers.
	 */
	void SetTimeout(double timeout);

	/**
	 * @return timeout_ the current timeout value for the WaitCommand
	 */
	double GetTimeout() const;

protected:

private:
	/**
	 * Push command controlls the Outake Function of the Intake Subsystem,
	 * responsable for moving the boulder from its storage.
	 */
	class Push: public Command
	{
	public:
		Push(Intake *intake);
		virtual ~Push() = default;

	public:
		/**
		 * Calls the Outake function of Intake to push the ball.
		 */
		void Initialize() override;

		/**
		 * Returns true if the command should no longet execute, false if
		 * otherwise. If the mechanical switch (detector_) on the Intake
		 * system is not pressed on, then the command finished execution.
		 */
		bool IsFinished() override;

		/**
		 * Execute is periodically called (every 20 ms) but does nothing.
		 */
		void Execute() override {};

		/**
		 * Does nothing as stopping the rollers is handled by Stopper
		 */
		void End() override {};

		/**
		 * Does nothing.
		 */
		void Interrupted() override {};

	private:
		Intake *intake_;
	};

	class Stopper: public Command
	{
	public:
		Stopper(Intake *intake);
		virtual ~Stopper() = default;

	public:
		/**
		 * Does nothing, as Stopper's only job is to stop the rollers
		 */
		void Initialize() override {};

		/**
		 * Function only returns true because the purpose is to stop
		 * the rollers.
		 */
		bool IsFinished() override;
		/**
		 * Execute is periodically called (every 20 ms) but does nothing.
		 */
		void Execute() override {};

		/**
		 * Calls the Stop function of Intake to stop moving the rollers
		 */
		void End() override;

		/**
		 * Does nothing.
		 */
		void Interrupted() override {};

	private:
		Intake *intake_;
	};

private:
	Intake *intake_;
	Push *push_;
	Stopper *stop_;
	double timeout_;
};

}// end namespace commands

#endif /* SRC_COMMANDS_PUSHBALL_H_ */
