#include "Subsystems/Swiss.h"
#include "WPILib.h"
#include <cmath>


using Swiss = subsystems::Swiss;

double Swiss::states[4] = {
		80.0 , 3.0 , 2.0 , 1.0
};

Swiss::Swiss(int deviceNumber) : Subsystem("Swiss"){
swisstalon = new CANTalon(deviceNumber);
swisstalon->SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogPot);

}



int Swiss::GetPosition(){


	return position;
}

void Swiss::MaxHeight(){
	swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
	swisstalon->Set(states[0]);
	//position = state[0];

}

void Swiss::MinHeight(){
	swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
		swisstalon->Set(states[3]);
	//position = state[3];

}


void Swiss::LiftDoor(){
	swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
	swisstalon->Set(states[1]);
	//position = state[1];

}

void Swiss::LowerCheval(){
	swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
	swisstalon->Set(states[2]);
	//position= state[2];

}
