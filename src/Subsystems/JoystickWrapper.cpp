/*
 * JoystickWrapper.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */


#include "JoystickWrapper.h"

#include "Utility/FunkyGet.h"

#include <string>
#include <memory>

namespace subsystems
{

using namespace dman;

JoystickWrapper::JoystickWrapper(std::string hand_name): WPISystem(std::move(hand_name))
{
}

void JoystickWrapper::doRegister()
{
	GetPortSpace("USB")("port");
}

bool JoystickWrapper::doConfigure()
{
	initJoystick();
	return is_initialized();
}

void JoystickWrapper::initJoystick()
{
	if(is_initialized())
		return;

	joy_ = std::make_unique<Joystick>(GetPortSpace("USB")("port"));

	// axes
	GetLocalValue<double>("axes/x").Initialize(std::make_shared<FunkyGet<double> >([this]()
		{
			return joy_->GetX();
		}));

	GetLocalValue<double>("axes/y").Initialize(std::make_shared<FunkyGet<double> >([this]()
		{
			return joy_->GetY();
		}));

	GetLocalValue<double>("axes/dial").Initialize(std::make_shared<FunkyGet<double> >([this]()
		{
			return joy_->GetZ();
		}));

	// buttons
	for(int b = 1; b <= 10; b++)
	{
		GetLocalValue<bool>("buttons/" + std::to_string(b)).Initialize(
				std::make_shared<FunkyGet<bool> >([this, b]()
					{
						return joy_->GetRawButton(b);
					}));
	}
}

}


