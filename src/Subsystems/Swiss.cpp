#include "Subsystems/Swiss.h"
#include "WPILib.h"
#include <cmath>



using Swiss = subsystems::Swiss;

using state_t = subsystems::Swiss::state_t;

double Swiss::tickToDegree = 90; //measure and change later
double Swiss::maxVelocity = .5; //measure and change later

double Swiss::states[] = {
		[retract] = .029,
		[horizontal] = .63,
		[cheval_down] = .7,
		[port_down] = .768
};


Swiss::Swiss() : Subsystem("Swiss"){
	swisstalon = new CANTalon(7);
	swisstalon->SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogPot);
	swisstalon->ConfigPotentiometerTurns(1);
	swisstalon->ConfigNeutralMode(CANSpeedController::kNeutralMode_Brake);
	swisstalon->ConfigForwardLimit(states[port_down]);
	swisstalon->ConfigReverseLimit(states[retract]);
	swisstalon->SetSensorDirection(false);
	swisstalon->SetClosedLoopOutputDirection(true);
	SetMode(pos);
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
	return static_cast<mode_t>(swisstalon->GetControlMode());
}
void Swiss::SetMode(mode_t m){
	if(GetMode() == m){
		return;
	}
	else if(m == pos){
		swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
		swisstalon->SelectProfileSlot(0);
		//swisstalon->SetPID(pPid.p, pPid.i, pPid.d);
		swisstalon->Set(swisstalon->Get());
		return;
	}
	else if(m == velocity){
		swisstalon->SetControlMode(CANTalon::ControlMode::kSpeed);
		swisstalon->SelectProfileSlot(1);
		SetVelocity(0.0, false);
		//swisstalon->SetPID(vPid.p, vPid.i, vPid.d);
		return;
	}
	else{
		swisstalon->SetControlMode(CANTalon::ControlMode::kPercentVbus);
		SetVelocity(0.0, false);
		return;
	}
}
state_t Swiss::GetState(){
	return position;
}

void Swiss::SetVelocity(double v, bool changeMode){
	if(changeMode)
		SetMode(velocity);

	if(GetMode() == mode_t::velocity)
		swisstalon->Set(maxVelocity * v);
	else if(GetMode() == mode_t::vbus)
		swisstalon->Set(v);
}
void Swiss::SetVoltage(double v){
	SetMode(vbus);
	swisstalon->Set(v);
}

void Swiss::SetState(state_t s){
	SetMode(pos);
	position = s;
	swisstalon->Set(states[position]);
}

