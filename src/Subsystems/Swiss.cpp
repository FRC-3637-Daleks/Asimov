#include "Subsystems/Swiss.h"
#include "WPILib.h"
#include <cmath>



using Swiss = subsystems::Swiss;

using state_t = subsystems::Swiss::state_t;

double Swiss::states[4] = {
		80.0 , 3.0 , 2.0 , 1.0
};

Swiss::Swiss(int deviceNumber) : Subsystem("Swiss"){
	swisstalon = new CANTalon(deviceNumber);
	swisstalon->SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogPot);
	swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
}



state_t Swiss::GetState(){
	return position;
}

void Swiss::MaxHeight(){

	SetState(max);
	//position = state[0];

}

void Swiss::MinHeight(){
	SetState(min);
	//position = state[3];

}


void Swiss::LiftDoor(){
	SetState(door);
	//position = state[1];

}

void Swiss::LowerCheval(){
	SetState(cheval);
	//position= state[2];

}

void Swiss::SetState(state_t s){
	position = s;
	swisstalon->Set(states[position]);
}

