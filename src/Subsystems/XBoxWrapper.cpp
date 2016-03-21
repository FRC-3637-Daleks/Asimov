/*
 * XBoxWrapper.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#include "XBoxWrapper.h"

#include "Utility/FunkyGet.h"

#include <string>
#include <memory>

namespace subsystems
{

using namespace dman;

std::string XBoxWrapper::ButtonToString(XBoxButton_t button)
{
	static const char * const button_strings[N_BUTTONS] =
		{"A", "B", "X", "Y", "L_TRIGGER", "R_TRIGGER", "BACK", "START", "L_STICK", "R_STICK"};
	if(button < 1 || button > N_BUTTONS)
		return "";

	return button_strings[button - 1];
}

std::string XBoxWrapper::AxisToString(XBoxAxis_t axis)
{
	static const char * const axis_strings[N_AXES] =
		{"L_X_Axis", "L_Y_Axis", "L_Trigger", "R_Trigger", "R_X_Axis", "R_Y_Axis"};
	if(axis < 0 || axis >= N_AXES)
		return "";

	return axis_strings[axis];
}

XBoxWrapper::XBoxWrapper(): WPISystem("xbox")
{
}

void XBoxWrapper::doRegister()
{
	GetPortSpace("USB")("port");
}

bool XBoxWrapper::doConfigure()
{
	initXBox();
	return is_initialized();
}

void XBoxWrapper::initXBox()
{
	if(is_initialized())
		return;

	xbox_ = std::make_unique<Joystick>(GetPortSpace("USB")("port"));

	// axes
	for(int a = 0; a < 6; a++)
	{
		GetLocalValue<double>(std::string("axes/") + AxisToString(static_cast<XBoxAxis_t>(a))).Initialize(std::make_shared<FunkyGet<double> >([this, a]()
			{
				return xbox_->GetRawAxis(a);
			}));
	}

	// pov
	GetLocalValue<int>(std::string("pov")).Initialize(std::make_shared<FunkyGet<int> >([this]()
		{
			return xbox_->GetPOV();
		}));

	// buttons
	for(int b = 1; b <= 10; b++)
	{
		GetLocalValue<bool>("buttons/" + ButtonToString(static_cast<XBoxButton_t>(b))).Initialize(
				std::make_shared<FunkyGet<bool> >([this, b]()
					{
						return xbox_->GetRawButton(b);
					}));
	}
}

}





