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
// Constructors & Destructors:
public:
	PushBall(Intake* intake);
	virtual ~PushBall() = default;

// Functions:
public:
	/**
	 * Not implemented. The constructor handles the addition of sequential
	 * commands and the initializaton of member variables.
	 */
	void Initialize() override {};

	/**
	 * Execute is periodically called (every 20 ms) but is not implemented.
	 */
	void Execute() override {};

	/**
	 * Not implemented.
	 */
	bool IsFinished() override {};

	/**
	 * Does nothing as stopping the rollers is handled by Stopper.
	 */
	void End() override {};

	/**
	 * Does nothing.
	 */
	void Interrupted() override {};

	/**
	 * Sets timeout_ to a specified timeout value, for the purpose of
	 * waiting to stop turning the rollers.
	 */
	void SetTimeout(double timeout);

	/**
	 * @return timeout_ the current timeout value for the WaitCommand.
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
		 * Execute is periodically called (every 20 ms) but not implemented.
		 */
		void Execute() override {};

		/**
		 * Not implemented as stopping the rollers is handled by Stopper.
		 */
		void End() override {};

		/**
		 * Not implemented.
		 */
		void Interrupted() override {};

	private:
		Intake *intake_;
	};

	/**
	 * Stopper's only responsability is to stop the front roller's movement
	 */
	class Stopper: public Command
	{
	public:
		Stopper(Intake *intake);
		virtual ~Stopper() = default;

	public:
		/**
		 * Not implemented.
		 */
		void Initialize() override {};

		/**
		 * Function only returns true because the purpose is to stop
		 * the rollers.
		 */
		bool IsFinished() override;
		/**
		 * Execute is periodically called (every 20 ms) but is not implemented.
		 */
		void Execute() override {};

		/**
		 * Calls the Stop function of Intake to stop moving the rollers
		 */
		void End() override;

		/**
		 * Not implemented.
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
