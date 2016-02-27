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

#include <memory>
#include <math.h>

namespace subsystems
{

using namespace dman;

OI::OI(): WPISystem("OI"), driver_left_("driver_left"), driver_right_("driver_right"),
		deadzone_(.05), forward_(true),
		tank_multiplier_(.5), forward_multiplier_(.5), turn_multiplier_(.5)
{
	AddSubSystem("driver_left", driver_left_);
	AddSubSystem("driver_right", driver_right_);
	AddSubSystem(copilot_.get_name(), copilot_);

	tank_left_ = GetLocalValue<double>("driver_left/axes/y");
	tank_right_ = GetLocalValue<double>("driver_right/axes/y");
	arcade_forward_ = GetLocalValue<double>("driver_right/axes/y");
	arcade_turn_ = GetLocalValue<double>("driver_left/axes/x");
	swiss_ = GetLocalValue<double>("xbox/axes/L_Y_Axis");
}

void OI::doRegister()
{
	auto& settings = GetSettings();
	settings("deadzone").SetDefault(get_deadzone());

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

}

bool OI::doConfigure()
{
	auto& settings = GetSettings();
	SetDeadzone(settings("deadzone").GetValueOrDefault());
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
	if(forward_)
		return transformAxis(tank_left_.GetValue(), true, get_tank_multiplier());
	return transformAxis(tank_right_.GetValue(), true, -get_tank_multiplier());
}

double OI::GetTankRight() const
{
	if(forward_)
		return transformAxis(tank_right_.GetValue(), true, get_tank_multiplier());
	return transformAxis(tank_left_.GetValue(), true, -get_tank_multiplier());
}

double OI::GetArcadeForward() const
{
	if(forward_)
		return transformAxis(arcade_forward_.GetValue(), true, get_forward_multiplier());
	return transformAxis(arcade_forward_.GetValue(), true, -get_forward_multiplier());
}

double OI::GetArcadeTurn() const
{
	return transformAxis(arcade_turn_.GetValue(), true, get_turn_multiplier());
}

double OI::GetSwiss() const
{
	return transformAxis(swiss_.GetValue(), false, 1.0);
}

commands::ForwardBoost *OI::MakeForwardBoost(double power)
{
	return new commands::ForwardBoost(this, power);
}

commands::TurnBoost *OI::MakeTurnBoost(double power)
{
	return new commands::TurnBoost(this, power);
}

}
