/*
 * Align.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: 3637-DS-1
 */
#include "Subsystems/Align.h"
#include "WPILib.h"
#include <cmath>



using Align = subsystems::Align;

Align::Align():Subsystem("Align"){
	Sensor1 = new AnalogInput(3);
	Sensor2 = new AnalogInput(0);
	Distance1 = 0;
	Distance2 = 0;
	TargetLeft = 10;
	TargetRight = 10;

}
double Align::GetDistance1(){
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
double Align::GetDistance2(){
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
double Align::LeftError(){
	return TargetLeft - GetDistance2();
}
double Align::RightError(){
	return TargetRight - GetDistance1();
}
double Align::ForwardError(){
	return (LeftError()+RightError())/2;
}
double Align::RotationError(){
	return (RightError()-LeftError());
}

double Align::GetLeftOutput(){
	return LeftError()/3.0;
}

double Align::GetRightOutput(){
	return RightError()/3.0;
}
