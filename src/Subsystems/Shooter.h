/*
 * Shooter.h
 *
 *  Created on: Feb 1, 2016
 *      Author: David
 */

#ifndef SRC_SUBSYSTEMS_SHOOTER_H_
#define SRC_SUBSYSTEMS_SHOOTER_H_
#include "WPILib.h"
#include "WPILib/WPISystem.h"

/**
 * Has forward declarations of command classes.
 */
namespace commands
{
	class SpinUp;
	class SpinDown;
}// end namespace commands

/**
 * Namespace for carrying Shooter class definition
 * over to other files.
 */
namespace subsystems
{

class Shooter : public dman::WPISystem
{
// Constructor & destructor:
public:
	Shooter();
	virtual ~Shooter();

// Enumerations:
public:
	/**
	 * Enumeration for possible Shooter states.
	 */
	enum class State_t {OFF = 0, SPINNINGUP, SPUNUP, SHOOTING, SPINNINGDOWN};

	/**
	 * Enumeration for possible top roller control modes.
	 */
	enum class Mode_t {VELOCITY = 0, VBUS};

// Dalek manager config functions:
public:
	/**
	 * Register Dalek Manager settings to default values for Shooter
	 * variables. Instant changes to settings can be made from a GUI
	 * without code recomplication.
	 */
	void doRegister() override;

	/**
	 * Initializes roller if not done so already, and configures the
	 * variables that have registered settings to that setting's
	 * current or default value.
	 * @return true
	 */
	bool doConfigure() override;

// Main functions:
public:
	/**
	 * Initializes settings of the top roller by calling CANTalon
	 * functions. The roller sets a Quadrature Encoder as its feedback
	 * device, sets maximum and minimum voltage output, and limits the
	 * roller to spin in only one (positive) direction.
	 */
	void Initialize();

	/**
	 * Controls the shooting roller by setting its speed and spinning it
	 * in an upward direction. If Shooter is in VELOCITY mode, the speed
	 * parameter is multiplied by the maximum velocity for scaling. If
	 * in VBUS mode, the roller's speed is set to the parameter.
	 *
	 * @param speed the scaled speed of the roller (double between -1.0
	 * and 1.0. defaulted to 1.0).
	 */
	void SpinUp(double speed = 1.0);

	/**
	 * Stops the shooting roller gradually by slowing it down.
	 */
	void SpinDown();

	/**
	 * Function to completely disable the front roller's controlling motor
	 * in an emergency.
	 */
	void EmergencyStop();

	/**
	 * @return max_velocity_ the maximum roller velocity
	 */
	double GetMaxVelocity() const;

	/**
	 * Sets the max velocity of the roller to a new value
	 * @param max_velocity the new maximum roller velocity
	 */
	void SetMaxVelocity(double max_velocity);

	/**
	 * @return the current speed of the top roller
	 */
	double GetSpeed() const;

// Error functions:
public:
	/**
	 * Returns the current error of the roller.
	 */
	double GetErr() const;

	/**
	 * Sets the allowed error of the roller.
	 */
	void SetAllowedError(double err);

	/**
	 * Returns the allowed error of the roller.
	 */
	double GetAllowedError() const;

	/**
	 * Returns whether the current error is greater than the allowable
	 * error, false if otherwise. Used to determine whether to stop the
	 * SpinUp and SpinDown commands.
	 */
	bool IsAllowable() const;

// State functions:
public:
	/**
	 * @return state_ the current state of Shooter.
	 */
	State_t GetState() const;

	/**
	 * Sets the current state of Shooter to parameter state
	 * @param state the new state of Shooter
	 */
	void SetState(State_t state);

// Control mode functions:
public:
	/**
	 * @return mode_ the top roller's current control mode
	 */
	Mode_t GetMode() const;

	/**
	 * Sets the control mode to a new one if the roller isn't
	 * already on that mode.
	 * @param mode the new mode of the top roller
	 */
	void SetMode(Mode_t mode);

// Command functions:
public:
	/**
	 * Creates new SpinUp command, and passes class instance as argument.
	 */
	commands::SpinUp * MakeSpinUp(double speed = 1.0);

	/**
	 * Creates new SpinDown command, and passes class instance as argument.
	 */
	commands::SpinDown * MakeSpinDown();

protected:

private:
	CANTalon *top_roller_;
	double allowed_error_;
	double max_velocity_;
	State_t state_;
	Mode_t mode_;
};

}// end namespace subsystems

#endif /* SRC_SUBSYSTEMS_SHOOTER_H_ */
