#include "Subsystems/Swiss.h"
#include "WPILib.h"
#include <cmath>



using Swiss = subsystems::Swiss;

using state_t = subsystems::Swiss::state_t;

double Swiss::tickToDegree = 90; //measure and change later
double Swiss::maxVelocity = .5; //measure and change later

double Swiss::states[] = {90, 50, 20, 0, 10};


Swiss::Swiss(int deviceNumber, double P, double I, double D) : Subsystem("Swiss"){
	swisstalon = new CANTalon(deviceNumber);
	swisstalon->SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogPot);
	swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
	swisstalon->ConfigPotentiometerTurns(1);
	swisstalon->SetPID(P, I, D);
	swisstalon->ConfigNeutralMode(CANSpeedController::kNeutralMode_Brake);
	swisstalon->ConfigForwardLimit(max);
	swisstalon->ConfigReverseLimit(min);

};

void Swiss::SetPIDValues(double p1, double p2, double p3, double v1, double v2, double v3){
	pPid.p = p1;
	pPid.i = p2;
	pPid.d = p3;
	vPid.p = v1;
	vPid.i = v2;
	vPid.d = v3;

}
Swiss::mode_t Swiss::GetMode(){
	return mode;
}
void Swiss::SetMode(mode_t m){
	if(mode == m){
		return;
	}
	else if(m == pos){
		swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
		mode = m;
		swisstalon->SelectProfileSlot(0);
		swisstalon->SetPID(pPid.p, pPid.i, pPid.d);
		return;
	}
	else if(m == velocity){
		swisstalon->SetControlMode(CANTalon::ControlMode::kSpeed);
		mode = m;
		swisstalon->SelectProfileSlot(1);
		swisstalon->SetPID(vPid.p, vPid.i, vPid.d);
		return;
	}
	else{
		swisstalon->SetControlMode(CANTalon::ControlMode::kPercentVbus);
		mode = m;
		return;
	}
}
state_t Swiss::GetState(){
	return position;
}

void Swiss::SetVelocity(double v, bool changeMode){
	if(changeMode)
		SetMode(velocity);

	if(mode == mode_t::velocity)
		swisstalon->Set(maxVelocity * v);
	else if(mode == mode_t::vbus)
		swisstalon->Set(v);
}

double Swiss::GetPos(){
	return swisstalon->GetPosition();
}

double Swiss::GetDiff(){
	if(GetMode()== Swiss::mode_t::pos){
		return GetState()-GetPos();
	}
	else{
		return 0;
	}


}

void Swiss::SetVoltage(double v){
	SetMode(vbus);
	swisstalon->Set(v);
}
void Swiss::MaxHeight(){
	SetState(max);
	//position = state[0];
}

void Swiss::MinHeight(){
	SetState(min);
	//position = state[3];

}

void Swiss::LowerPort(){
	SetState(doordown);
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
	SetMode(pos);
	position = s;
	swisstalon->Set(states[position]);
}

bool Swiss::IsCloseNuff(){
	if (GetDiff()> 5){
		return false;
	}
	else{
		return true;
	}
}
void Swiss::Hold(){
	swisstalon->Set(GetPos());
}

