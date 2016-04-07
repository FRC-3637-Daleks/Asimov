/*
 * Drive.h
 *
 *  Created on: Jan 30, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_DRIVE_H_
#define SRC_SUBSYSTEMS_DRIVE_H_

// WPI Includes
#include "WPILib.h"

// Dalek Manager Includes
#include "WPILib/WPISystem.h"

// STD Includes
#include <memory>
#include <string>
#include <utility>

#define PI 3.1415926535897

namespace commands
{

class DriveStraight;
class TankDrive;
class ArcadeDrive;
class Turn;

}

namespace subsystems
{
class Drive: public dman::WPISystem
{
public:
	enum class Mode_t: uint8_t {
		VBus,	///< -1.0 to 1.0 of max motor power
		Velocity,	///< Velocity. Use for varying terrain
		Position	///< Positional. Used in auto
	};

	enum ModePIDSlot_t: uint8_t {
		VelocityPID = 0,
		PositionPID = 1
	};

	using Meters_t = double;
	using MetersPerSecond_t = double;
	template<class TalonsT> using Ptr_t = std::unique_ptr<TalonsT>;
	using Ticks_t = uint32_t;

public:
	static std::string ModeToString(Mode_t mode);

public:
	Drive();
	virtual ~Drive() = default;

public:  /// Configuration functions
	bool is_initialized() const {return talons_ != nullptr;}
	void doRegister() override;
	bool doConfigure() override;

	void SetMode(Mode_t m);
	Mode_t get_mode() const {return mode_;}

	/// Used positional calculations
	void SetTicksPerRev(Ticks_t val);
	Ticks_t get_ticks_per_rev() const {return ticks_per_rev_;}

	/// Used for distance calculations
	void SetWheelDiameter(Meters_t val);
	Meters_t get_wheel_diameter() const {return wheel_diameter_;}
	Meters_t get_wheel_circumference() const {return get_wheel_diameter() * PI;}

	/// Standardizes velocities to be on a -1.0 to 1.0 scale
	void SetVelocityScale(MetersPerSecond_t max_velocity);
	MetersPerSecond_t get_velocity_scale() const {return max_velocity_;}

	/// Used for point-turning robot
	void SetWheelRevsPerBaseRev(double rate);
	double get_wheel_revs_per_base_rev() const {return wheel_revs_per_base_rev_;}

	void SetAllowableError(double allow);
	double get_allowable_error() {return allowable_error_;}

public:  /// Position tracking functions

	/// Returns distance traveled of robot in meters based on average of right and left wheel revs
	Meters_t GetDistance() const;

	/** Returns rotation rotated by robot in proportion of revolutions based on right and left wheel revs
	 * In the future this may also return a sensor which returns heading
	 */
	double GetRotation() const;

	/// Returns number of revolutions on a side
	double GetLeftRevs() const;
	double GetRightRevs() const;

	/// Returns rpm for a side
	double GetLeftRPM() const;
	double GetRightRPM() const;

	// Returns setpoints for a side
	double GetLeftSetPointRPM() const;
	double GetRightSetPointRPM() const;

	/// Returns velocity of robot in meters per second based on average of right and left wheel revs per second
	MetersPerSecond_t GetVelocity() const;

	/// Returns turning velocity of robot in base revolutions per minute
	double GetBaseRPM() const;

	/// Sets both motor revs to 0
	void ResetPosition();

	/** Sets both sides to go to \c pos
	 * @return true once both motors are considered to be at the position
	 */
	bool GoToPosition(Meters_t pos);

	/** Sets both sides to turn opposite directions to point-turn the base \c rotate revolutions
	 * @return true once both motors are considered to be at the right rotation
	 */
	bool TurnToRotation(double revolutions);

public:	/// Drive functions

	/** Sets left side to left speed and right side to right speed
	 * If the system is in positional mode it will do nothing
	 * If the system is in velocity mode it will scale the parameters by 1:max_output
	 * @param left -1.0 to 1.0 proportion of left side velocity
	 * @param right -1.0 to 1.0 proportion of right side velocity
	 */
	void TankDrive(double left, double right);

	/** Sets forward velocity to y and turning velocity to rotation
	 * Essentially calls TankDrive(y+rotation, y-rotation)
	 */
	void ArcadeDrive(double y, double rotation);

	/** Stops
	 */
	void Stop();

public:  // Command Generation
	/** Forwards the arguments to a DriveStraight constructor with this as the first arg
	 * @see commands::DriveStraight::DriveStraight
	 */
	template<class ... Types>
	commands::DriveStraight * MakeDriveStraight(Types ... args);

	/** Forwards the arguments to a TankDrive constructor with this as the first arg
	 * @see commands::TankDrive::TankDrive
	 */
	template<class ... Types>
	commands::TankDrive * MakeTankDrive(Types ... args);

	/** Forwards the arguments to an ArcadeDrive constructor with this as the first arg
	 * @see commands::ArcadeDrive::ArcadeDrive
	 */
	template<class ... Types>
	commands::ArcadeDrive * MakeArcadeDrive(Types ... args);

	/** Forwards the arguments to a Turn constructor with this as the first arg
	 * @see commands::Turn::Turn
	 */
	template<class ... Types>
	commands::Turn * MakeTurn(Types ... args);

private:
	struct Talons
	{
		Talons(uint8_t left, uint8_t left_slave,
			   uint8_t right, uint8_t right_slave):
			left_(left), left_slave_(left_slave),
			right_(right), right_slave_(right_slave) {}
		CANTalon left_;
		CANTalon left_slave_;
		CANTalon right_;
		CANTalon right_slave_;
	};

private:
	// Creates the talons if they haven't been already
	void initTalons();

	// Configures the motor at master for the current mode
	bool configureMaster(CANTalon &master);

	// Runs configureMaster on both sides
	bool configureBoth();

	void setModeMaster(CANTalon &master);

private:
	Ptr_t<Talons> talons_ ;
	Mode_t mode_;
	Ticks_t ticks_per_rev_;
	Meters_t wheel_diameter_;
	MetersPerSecond_t max_velocity_;
	double wheel_revs_per_base_rev_;
	Meters_t allowable_error_;
};


}

// Drive class dependent files
#include "Commands/DriveStraight.h"
#include "Commands/TankDrive.h"
#include "Commands/ArcadeDrive.h"
#include "Commands/Turn.h"

template<class ... Types>
commands::DriveStraight * subsystems::Drive::MakeDriveStraight(Types ... args)
{
	return new commands::DriveStraight(this, std::forward<Types>(args)...);
}

template<class ... Types>
commands::TankDrive * subsystems::Drive::MakeTankDrive(Types ... args)
{
	return new commands::TankDrive(this, std::forward<Types>(args)...);
}

template<class ... Types>
commands::ArcadeDrive * subsystems::Drive::MakeArcadeDrive(Types ... args)
{
	return new commands::ArcadeDrive(this, std::forward<Types>(args)...);
}

template<class ... Types>
commands::Turn * subsystems::Drive::MakeTurn(Types ... args)
{
	return new commands::Turn(this, std::forward<Types>(args)...);
}


#endif /* SRC_SUBSYSTEMS_DRIVE_H_ */
