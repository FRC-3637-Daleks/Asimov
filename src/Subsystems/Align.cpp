/*
 * Align.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: 3637-DS-1
 */
#include "Subsystems/Align.h"
#include "WPILib.h"
#include <cmath>

#include "Utility/FunkyGet.h"

using Align = subsystems::Align;

Align::Align(): WPISystem("Align"){
	Sensor1 = new AnalogInput(3);
	Sensor2 = new AnalogInput(0);
	Distance1 = 0;
	Distance2 = 0;
	TargetLeft = 10;
	TargetRight = 10;

}
double Align::GetDistance1() const{
	double Volt = static_cast<double>(Sensor1->GetVoltage());
	if (Volt<=1.77){
		return 1/((Volt - .65)/(13.21208267) + .05) - .42;
	}
	else if(Volt <=2.34){
		return 1/((Volt - 2.34)/(8.670916) + .1845) - .42;
	}
	else{
		return 1/((Volt - 2.34)/(9.341717+.1845)) - .42;
	}

}
double Align::GetDistance2() const {
	double Volt = static_cast<double>(Sensor2->GetVoltage());
	if (Volt<=1.77){
		return 1/((Volt - .65)/(13.21208267) + .05) - .42;
	}
	else if(Volt <=2.34){
		return 1/((Volt - 2.34)/(8.670916) + .1845) - .42;
	}
	else{
		return 1/((Volt - 2.34)/(9.341717+.1845)) - .42;
	}
}
void Align::SetRightTarget(double tgt){
	TargetRight = tgt;
}
void Align::SetLeftTarget(double tgt){
	TargetLeft = tgt;
}
double Align::LeftError() const{
	return TargetLeft - GetDistance2();
}
double Align::RightError() const {
	return TargetRight - GetDistance1();
}
double Align::ForwardError() const {
	return (LeftError()+RightError())/2;
}
double Align::RotationError() const {
	return (RightError()-LeftError());
}

double Align::GetLeftOutput() const {
	return LeftError()/3.0;
}

double Align::GetRightOutput() const {
	return RightError()/3.0;
}

void Align::doRegister()
{
	{
		auto& ports = GetPortSpace("AnalogIn");
		ports("right").SetDefault(3);
		ports("left").SetDefault(0);
	}

	{
		auto& settings = GetSettings();
		settings("target").SetDefault(TargetLeft);
	}

	GetLocalValue<double>("left_error").Initialize(std::make_shared<FunkyGet<double> >([this]()
			{
				return LeftError();
			}));
	GetLocalValue<double>("right_error").Initialize(std::make_shared<FunkyGet<double> >([this]()
			{
				return RightError();
			}));
	GetLocalValue<double>("forward_error").Initialize(std::make_shared<FunkyGet<double> >([this]()
			{
				return ForwardError();
			}));
	GetLocalValue<double>("rotation_error").Initialize(std::make_shared<FunkyGet<double> >([this]()
			{
				return RotationError();
			}));
	GetLocalValue<double>("left_output").Initialize(std::make_shared<FunkyGet<double> >([this]()
			{
				return GetLeftOutput();
			}));
	GetLocalValue<double>("right_output").Initialize(std::make_shared<FunkyGet<double> >([this]()
			{
				return GetRightOutput();
			}));
}

bool Align::doConfigure()
{
	initSensors();

	auto& target = GetSettings()("target");
	SetLeftTarget(target.GetValueOrDefault<double>());
	SetRightTarget(target.GetValueOrDefault<double>());

	return true;
}


void Align::initSensors()
{
	if(is_initialized())
		return;

	auto& ports = GetPortSpace("AnalogIn");

	Sensor1 = new AnalogInput(ports("right"));
	Sensor2 = new AnalogInput(ports("left"));
}
