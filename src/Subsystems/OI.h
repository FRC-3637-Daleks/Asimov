/*
 * OI.h
 *
 *  Created on: Feb 26, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_OI_H_
#define SRC_SUBSYSTEMS_OI_H_

#include "JoystickWrapper.h"
#include "XBoxWrapper.h"

#include "Utility/ValueStore.h"
#include "WPILib/WPISystem.h"

#include "WPILib.h"

#include <memory>

namespace commands
{

class ForwardBoost;
class TurnBoost;

}

namespace subsystems
{

using namespace dman;

class OI: public WPISystem
{
public:
	using AxisValue_t = ValueStore::Value<double>;
	using ButtonValue_t = ValueStore::Value<bool>;

public:
	OI();
	virtual ~OI() = default;

public:
	void doRegister() override;
	bool doConfigure() override;

public:  // Settings
	double get_deadzone() const {return deadzone_;}
	void SetDeadzone(double deadzone) {deadzone_ = deadzone;}

	bool get_forward() const {return forward_;}
	void SetForward(bool forward) {forward_ = forward;}

	double get_tank_multiplier() const {return tank_multiplier_;}
	void SetTankMultiplier(double tank_multiplier) {tank_multiplier_ = tank_multiplier;}

	double get_forward_multiplier() const {return forward_multiplier_;}
	void SetForwardMultiplier(double forward_multiplier) {forward_multiplier_ = forward_multiplier;}

	double get_turn_multiplier() const {return turn_multiplier_;}
	void SetTurnMultiplier(double turn_multiplier) {turn_multiplier_ = turn_multiplier;}

public:
	// Drive Axes
	double GetTankLeft() const;
	double GetTankRight() const;
	double GetArcadeForward() const;
	double GetArcadeTurn() const;

	// Copilot Axes
	double GetSwiss() const;

	// Copilot Buttons
	bool GetIntake() const;
	bool GetPush() const;
	bool GetShoot() const;
	bool GetSpinUp() const;

public:
	// Drive Axis Values
	AxisValue_t GetTankLeftAxis() {return GetLocalValue<double>("tank_left");}
	AxisValue_t GetTankRightAxis() {return GetLocalValue<double>("tank_right");}
	AxisValue_t GetArcadeForwardAxis() {return GetLocalValue<double>("arcade_forward");}
	AxisValue_t GetArcadeTurnAxis() {return GetLocalValue<double>("arcade_turn");}

	// Copilot Axis Values
	AxisValue_t GetSwissAxis() {return GetLocalValue<double>("swiss");}

public:
	Joystick& get_left() {return driver_left_.get_joy();}
	Joystick& get_right() {return driver_right_.get_joy();}
	Joystick& get_xbox() {return copilot_.get_joy();}

public:  // Command Generation
	commands::ForwardBoost *MakeForwardBoost(double power);
	commands::TurnBoost *MakeTurnBoost(double power);

private:
	double transformAxis(double from, bool squared = false, double multiplier = 1.0) const;

private:
	JoystickWrapper driver_left_, driver_right_;
	XBoxWrapper copilot_;
	AxisValue_t tank_left_, tank_right_, arcade_forward_, arcade_turn_;
	AxisValue_t swiss_;
	double deadzone_;
	bool forward_;
	double tank_multiplier_, forward_multiplier_, turn_multiplier_;
};

}



#endif /* SRC_SUBSYSTEMS_OI_H_ */
