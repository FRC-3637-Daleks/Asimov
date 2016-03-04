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
#include "Subsystems/Swiss.h"

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
	using POVvalue_t = ValueStore::Value<int>;

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

	// Drive Buttons
	bool GetForwardBoost() const {return forward_boost_.GetValueOr(false);}
	bool GetTurnBoost() const {return turn_boost_.GetValueOr(false);}
	bool GetTankMode() const {return tank_mode_.GetValueOr(false);}
	bool GetArcadeMode() const {return arcade_mode_.GetValueOr(false);}
	bool GetForwardSensorAlign() const {return forward_sensor_align_.GetValueOr(false);}
	bool GetTurnSensorAlign() const {return turn_sensor_align_.GetValueOr(false);}
	bool GetFrontMode() const {return front_mode_.GetValueOr(false);}
	bool GetReverseMode() const {return reverse_mode_.GetValueOr(false);}

	// Copilot Buttons
	bool GetIntake() const {return intake_.GetValueOr(false);}
	bool GetPush() const {return push_.GetValueOr(false);}
	bool GetShoot() const {return shoot_.GetValueOr(false);}
	bool GetSpinUp() const {return spinup_.GetValueOr(false);}
	bool GetSpinDown() const {return spindown_.GetValueOr(false);}
	bool GetForceToggle() const {return force_toggle_.GetValueOr(false);}

	// Misc.
	Swiss::state_t GetSwissPosition() const;
	bool IsSwissState(Swiss::state_t state) const {return GetSwissPosition() == state;}
	bool GetSwissControl() const {return fabs(swiss_.GetValueOr(0.0)) > get_deadzone();}

public:
	// Drive Axis Values
	AxisValue_t GetTankLeftAxis() {return GetLocalValue<double>("tank_left");}
	AxisValue_t GetTankRightAxis() {return GetLocalValue<double>("tank_right");}
	AxisValue_t GetArcadeForwardAxis() {return GetLocalValue<double>("arcade_forward");}
	AxisValue_t GetArcadeTurnAxis() {return GetLocalValue<double>("arcade_turn");}
	AxisValue_t GetNullAxis() {return GetLocalValue<double>("null_axis");}

	// Copilot Axis Values
	AxisValue_t GetSwissAxis() {return GetLocalValue<double>("swiss");}

	// Triggers
	ButtonValue_t GetForwardBoostButton() {return GetLocalValue<bool>("forward_boost");}
	ButtonValue_t GetTurnBoostButton() {return GetLocalValue<bool>("turn_boost");}
	ButtonValue_t GetTankModeButton() {return GetLocalValue<bool>("tank_mode");}
	ButtonValue_t GetArcadeModeButton() {return GetLocalValue<bool>("arcade_mode");}
	ButtonValue_t GetForwardSensorAlignButton() {return GetLocalValue<bool>("forward_sensor_align");}
	ButtonValue_t GetTurnSensorAlignButton() {return GetLocalValue<bool>("turn_sensor_align");}
	ButtonValue_t GetFrontModeButton() {return GetLocalValue<bool>("front_mode");}
	ButtonValue_t GetReverseModeButton() {return GetLocalValue<bool>("reverse_mode");}
	ButtonValue_t GetIntakeButton() {return GetLocalValue<bool>("intake");}
	ButtonValue_t GetPushButton() {return GetLocalValue<bool>("push");}
	ButtonValue_t GetShootButton() {return GetLocalValue<bool>("shoot");}
	ButtonValue_t GetSpinUpButton() {return GetLocalValue<bool>("spin_up");}
	ButtonValue_t GetSpinDownButton() {return GetLocalValue<bool>("spin_down");}
	ButtonValue_t GetForceToggleButton() {return GetLocalValue<bool>("force_toggle");}
	ButtonValue_t GetSwissStateButton(Swiss::state_t state);
	ButtonValue_t GetControlSwissButton() {return GetLocalValue<bool>("swiss_control");}

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
	bool force_;
	double tank_multiplier_, forward_multiplier_, turn_multiplier_;

private:  // Buttons
	ButtonValue_t forward_boost_, turn_boost_;
	ButtonValue_t tank_mode_, arcade_mode_;
	ButtonValue_t forward_sensor_align_, turn_sensor_align_;
	ButtonValue_t front_mode_, reverse_mode_;
	ButtonValue_t intake_, push_, shoot_;
	ButtonValue_t spindown_, spinup_;
	ButtonValue_t force_toggle_;

private:  // POV
	POVvalue_t swiss_state_;
};

}



#endif /* SRC_SUBSYSTEMS_OI_H_ */
