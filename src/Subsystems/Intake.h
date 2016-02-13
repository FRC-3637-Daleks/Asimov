/*
 * Intake.h
 *
 *  Created on: Jan 27, 2016
 *      Author: David
 */

#ifndef SRC_SUBSYSTEMS_INTAKE_H_
#define SRC_SUBSYSTEMS_INTAKE_H_
#include "WPILib.h"

/**
 * Has forward declarations of Intake Command classes.
 */
namespace commands
{
	class IntakeBall;
	class PushBall;
} // end namespace commands

/**
 * Namespace for carying Intake class definition over
 * to other files.
 */
namespace subsystems
{

class Intake : public Subsystem
{
// Constructor & destructor:
public:
	Intake();
	virtual ~Intake();

// Enumerations // TODO ::: DOCUMENTATION
public:
	enum class State_t {OFF = 0, TAKING, HOLDING, PUSHING, SHOOTING};
	enum class Mode_t {VELOCITY = 0, VBUS};

// Main functions:
public:
	/**
	 * Initializes the settings of the front roller. The roller sets
	 * a Quadrature Encoder as its feedback device, and sets
	 * maximum and minumum voltage rates.
	 */
	void Initialize();

	/**
	 * Returns true if the mechanical switch (detector_) is pressed
	 * on, and false otherwise. Used to determine whether or not to
	 * call Stop(), and if commands should end.
	 */
	bool CheckSwitch() const;

	/**
	 * Controls the front rollers to take in and store the boulder.
	 * Calls SetSpeed to conrol the roller's speed, and CheckSwitch
	 * to determine if the rollers should be stopped. If CheckSwitch
	 * and variable check are both true, Stop is called.
	 * NOTE: Relies on the IntakeBall command
	 *
	 * @param check is used as a safety measure determine if the
	 * CheckSwitch function should be called just in case the switch
	 * itself malfunctions in the field. Check is defaulted to true.
	 */
	void TakeBall(bool check = true);

	/**
	 * Controls the front rollers in order to horizontally push the
	 * boulder from its storage. It will call the SetSpeed function to
	 * control the speed of the rollers.
	 * NOTE: OutakeBall is controlled by the PushBall command.
	 */
	void OutakeBall();

	/**
	 * Sets the speed and direction of the front roller. Values of -1 or
	 * 1 for speed represent the roller's maximum possible speed in two
	 * dirtectioms. Negative values indicate that the roller should move
	 * outward (push ball) while positive values indicate the opposite.
	 *
	 * @param speed the speed of the roller, a double between -1.0 and 1.0
	 */
	void SetSpeed(double speed);

	/**
	 * Returns the front roller's current speed ( value between -1 and 1)
	 */
	double GetSpeed() const;

	/**
	 * Calls SetSpeed with argument 0.0 to stop rollers. Only called by either
	 * TakeBall or when the IntakeBall command's End function is invoked.
	 */
	void Stop();

	/**
	 * Sets the intake_speed_ (auto) variable to a new speed (-1 to 1)
	 * @param intake_speed the new automatic intake speed
	 */
	void SetIntakeSpeed(double intake_speed);

	/**
	 * Sets the push_speed (auto) variable to a new speed (-1 to 1)
	 * @param push_speed the new automatic push speed
	 */
	void SetPushSpeed(double push_speed);

	/**
	 * @return intake_speed_ the current automatic intake speed
	 */
	double GetIntakeSpeed() const;

	/**
	 * @return push_speed_ the current automatic push speed
	 */
	double GetPushSpeed() const;

	// TODO ::: DOCUMENTATION
	double GetMaxVelocity() const;
	void SetMaxVelocity(double max_velocity);

// Error Functions:
public:
		/**
		 * Returns the current error of the roller.
		 */
		double GetErr() const;

		/**
		 * Sets the allowed error of the roller.
		 * @param err the new allowable error
		 */
		void SetAllowedError(double err);

		/**
		 * Returns the allowed error of the roller.
		 */
		double GetAllowedError() const;

		/**
		 * Returns whether the current error is greater than the allowable
		 * error, false if otherwise.
		 */
		bool IsAllowable() const;

// State functions:
public:
	/**
	 * @return state_ the current state of Intake
	 */
	State_t GetState() const;

	/**
	 * Sets the current state of Intake to state.
	 * @param state the new state of Intake.
	 */
	void SetState(State_t state);

// Mode functions: TODO ::: DOCUMENTATION
public:
	Mode_t GetMode() const;
	void SetMode(Mode_t mode);

// Command functions:
public:
	/**
	 * Creates new IntakeBall command, and passes class instance as argument
	 */
	commands::IntakeBall * MakeIntakeBall();

	/**
	 * Creates new PushBall command, and passes class instance as argument
	 */
	commands::PushBall * MakePushBall();

protected:

private:
	CANTalon *roller_;
	DigitalInput *detector_;
	State_t state_;
	Mode_t mode_;
	double intake_speed_;
	double push_speed_;
	double allowed_error_;
	double max_velocity_;
};

} // end namespace subsystems

#endif /* SRC_SUBSYSTEMS_INTAKE_H_ */
