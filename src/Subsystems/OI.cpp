/*
 * OI.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: Edward
 */


#include "OI.h"

#include "Utility/FunkyGet.h"
#include "Config/PortSpace.h"

#include "Commands/ForwardBoost.h"
#include "Commands/TurnBoost.h"
#include "Commands/ShootMode.h"

#include <memory>
#include <math.h>

namespace subsystems
{

using namespace dman;

OI::OI(): WPISystem("OI"), driver_left_("driver_left"), driver_right_("driver_right"),
		deadzone_(.05), forward_(true),
		tank_multiplier_(1.0), forward_multiplier_(1.0), turn_multiplier_(1.0),
		base_tank_multiplier_(.5), base_forward_multiplier_(.5), base_turn_multiplier_(.5)
{
	AddSubSystem("driver_left", driver_left_);
	AddSubSystem("driver_right", driver_right_);
	AddSubSystem("xbox", copilot_);
}

void OI::doRegister()
{
	// axes
	tank_left_ = GetLocalValue<double>("driver_left/axes/y");
	tank_right_ = GetLocalValue<double>("driver_right/axes/y");
	arcade_forward_ = GetLocalValue<double>("driver_right/axes/y");
	arcade_turn_ = GetLocalValue<double>("driver_left/axes/x");
	swiss_ = GetLocalValue<double>("xbox/axes/L_Y_Axis");

	// triggers
	forward_boost_ = GetLocalValue<bool>("driver_right/buttons/1");
	turn_boost_ = GetLocalValue<bool>("driver_left/buttons/1");
	tank_mode_ = GetLocalValue<bool>("driver_right/buttons/6");
	arcade_mode_ = GetLocalValue<bool>("driver_right/buttons/7");
	forward_sensor_align_ = GetLocalValue<bool>("driver_right/buttons/8");
	turn_sensor_align_ = GetLocalValue<bool>("driver_left/buttons/8");
	front_mode_ = GetLocalValue<bool>("driver_right/buttons/11");
	reverse_mode_ = GetLocalValue<bool>("driver_right/buttons/10");
	intake_ = GetLocalValue<bool>("xbox/buttons/A");
	push_ = GetLocalValue<bool>("driver_right/buttons/3");
	shoot_ = GetLocalValue<bool>("xbox/buttons/R_TRIGGER");
	spindown_ = GetLocalValue<bool>("xbox/buttons/X");
	spinup_ = GetLocalValue<bool>("xbox/buttons/Y");
	force_toggle_ = GetLocalValue<bool>("xbox/buttons/BACK");

	// POV
	swiss_state_ = GetLocalValue<int>("xbox/pov");

	auto& settings = GetSettings();
	settings("deadzone").SetDefault(get_deadzone());

	{
		auto& speed_settings = settings["speed_settings"];
		speed_settings("base_tank_speed").SetDefault(base_tank_multiplier_);
		speed_settings("base_forward_speed").SetDefault(base_forward_multiplier_);
		speed_settings("base_turn_speed").SetDefault(base_turn_multiplier_);
	}

	GetTankLeftAxis().Initialize(std::make_shared<FunkyGet<double> >([this]()
		{
			return GetTankLeft();
		}));
	GetTankRightAxis().Initialize(std::make_shared<FunkyGet<double> >([this]()
		{
			return GetTankRight();
		}));
	GetArcadeForwardAxis().Initialize(std::make_shared<FunkyGet<double> >([this]()
		{
			return GetArcadeForward();
		}));
	GetArcadeTurnAxis().Initialize(std::make_shared<FunkyGet<double> >([this]()
		{
			return GetArcadeTurn();
		}));
	GetSwissAxis().Initialize(std::make_shared<FunkyGet<double> >([this]()
		{
			return GetSwiss();
		}));
	GetNullAxis().Initialize(std::make_shared<FunkyGet<double> >([]() {
		return 0.0;
	}));
	GetLocalValue<bool>("false").Initialize(std::make_shared<FunkyGet<bool> >([]() {
		return false;
	}));

	// triggers
	GetForwardBoostButton().Initialize(std::make_shared<FunkyGet<bool>>([this]() {
		return GetForwardBoost();
	}));
	GetTurnBoostButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetTurnBoost();
	}));
	GetTankModeButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetTankMode();
	}));
	GetArcadeModeButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetArcadeMode();
	}));
	GetForwardSensorAlignButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetForwardSensorAlign();
	}));
	GetTurnSensorAlignButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetTurnSensorAlign();
	}));
	GetFrontModeButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetFrontMode();
	}));
	GetReverseModeButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetReverseMode();
	}));
	GetIntakeButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetIntake();
	}));
	GetPushButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetPush();
	}));
	GetShootButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetShoot();
	}));
	GetSpinUpButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetSpinUp();
	}));
	GetSpinDownButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetSpinDown();
	}));
	GetForceToggleButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetForceToggle();
	}));
	GetControlSwissButton().Initialize(std::make_shared<FunkyGet<bool> >([this]() {
		return GetSwissControl();
	}));
}

bool OI::doConfigure()
{
	auto& settings = GetSettings();
	SetDeadzone(settings("deadzone").GetValueOrDefault<double>());

	{
		auto& speed_settings = settings["speed_settings"];
		base_forward_multiplier_ = speed_settings("base_forward_speed").GetValueOrDefault<double>();
		base_turn_multiplier_ = speed_settings("base_turn_speed").GetValueOrDefault<double>();
		tank_multiplier_ = speed_settings("base_tank_speed").GetValueOrDefault<double>();
	}


	Log(dman::MessageData::INFO, "", "Subsystem") << "Deadzone: " << get_deadzone();
	return true;
}

double OI::transformAxis(double from, bool squared, double multiplier) const
{
	if(fabs(from) < get_deadzone())
		return 0.0;

	if(squared) from *= fabs(from);
	return from *= multiplier;
}

double OI::GetTankLeft() const
{
	if(!tank_right_.initialized() && !tank_left_.initialized())
		return 0.0;

	if(forward_)
		return transformAxis(tank_left_.GetValue(), true, -get_tank_multiplier());
	return transformAxis(tank_right_.GetValue(), true, get_tank_multiplier());
}

double OI::GetTankRight() const
{
	if(!tank_right_.initialized() && !tank_left_.initialized())
		return 0.0;

	if(forward_)
		return transformAxis(tank_right_.GetValue(), true, -get_tank_multiplier());
	return transformAxis(tank_left_.GetValue(), true, get_tank_multiplier());
}

double OI::GetArcadeForward() const
{
	if(!arcade_forward_.initialized())
		return 0.0;

	if(forward_)
		return transformAxis(arcade_forward_.GetValue(), true, -get_forward_multiplier());
	return transformAxis(arcade_forward_.GetValue(), true, get_forward_multiplier());
}

double OI::GetArcadeTurn() const
{
	if(!arcade_turn_.initialized())
		return 0.0;
	return transformAxis(arcade_turn_.GetValue(), true, -get_turn_multiplier());
}

double OI::GetSwiss() const
{
	if(!swiss_.initialized())
		return 0.0;
	return transformAxis(swiss_.GetValue(), false, -1.0);
}

Swiss::state_t OI::GetSwissPosition() const
{
	double pov = swiss_state_.GetValueOr(-1);
	if(pov < 0)
		return Swiss::n_states;
	else if(pov < 91)
		return Swiss::horizontal;
	else if(pov < 150)
		return Swiss::cheval_down;
	else if(pov < 181)
		return Swiss::port_down;
	else if(pov < 271)
		return Swiss::retract;

	return Swiss::n_states;
}

OI::ButtonValue_t OI::GetSwissStateButton(Swiss::state_t state)
{
	if(state == Swiss::n_states)
	{
		return GetLocalValue<bool>("false");
	}

	auto ret = GetLocalValue<bool>(std::string("swiss_") + Swiss::StateToString(state));
	if(!ret.initialized())
	{
		ret.Initialize(std::make_shared<FunkyGet<bool> >([this, state]() {
			return IsSwissState(state);
		}));
	}

	return ret;
}

commands::ForwardBoost *OI::MakeForwardBoost(double power)
{
	return new commands::ForwardBoost(this, power);
}

commands::TurnBoost *OI::MakeTurnBoost(double power)
{
	return new commands::TurnBoost(this, power);
}

commands::ShootMode *OI::MakeShootMode(subsystems::Camera *camera, double forward_multiplier, double turn_multiplier)
{
	return new commands::ShootMode(this, camera, forward_multiplier, turn_multiplier);
}

}
