#include "Subsystems/Swiss.h"
#include "WPILib.h"
#include <cmath>



using Swiss = subsystems::Swiss;

using state_t = subsystems::Swiss::state_t;

double Swiss::tickToDegree = 90; //measure and change later
double Swiss::maxVelocity = 18;

<<<<<<< HEAD
double Swiss::states[] = {90, 50, .032, 0, 10};
=======
double Swiss::states[] = {
		[retract] = -.032,
		[horizontal] = -.63,
		[cheval_down] = -.7,
		[port_down] = -.768
};
>>>>>>> origin/dev-elazer


Swiss::Swiss() : Subsystem("Swiss"){
	swisstalon = new CANTalon(7);
	swisstalon->SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogPot);
	swisstalon->ConfigPotentiometerTurns(1);
	swisstalon->ConfigNeutralMode(CANSpeedController::kNeutralMode_Brake);
	swisstalon->ConfigReverseLimit(states[port_down]);
	swisstalon->ConfigForwardLimit(states[retract]);
	swisstalon->ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SoftPositionLimits);
	swisstalon->SetSensorDirection(true);
	swisstalon->SetClosedLoopOutputDirection(false);
	swisstalon->SetInverted(false);
	SetMode(pos);
};

Swiss::mode_t Swiss::GetMode(){
	return static_cast<mode_t>(swisstalon->GetControlMode());
}
<<<<<<< HEAD
void Swiss::SetMode(mode_t m, bool force){

	if(!force){
		if(mode == m){
					return;
		}
=======
void Swiss::SetMode(mode_t m){
	if(GetMode() == m){
		return;
>>>>>>> origin/dev-elazer
	}
	if(m == pos){
		swisstalon->SetControlMode(CANTalon::ControlMode::kPosition);
		swisstalon->SelectProfileSlot(0);
		swisstalon->Set(swisstalon->Get());
		return;
	}
	else if(m == velocity){
		swisstalon->SetControlMode(CANTalon::ControlMode::kSpeed);
		swisstalon->SelectProfileSlot(1);
		SetVelocity(0.0, false);
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
		SetMode(velocity, true);

	if(GetMode() == mode_t::velocity)
		swisstalon->Set(maxVelocity * v);
	else if(GetMode() == mode_t::vbus)
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
	SetMode(vbus, true);
	swisstalon->Set(v);
}

void Swiss::SetState(state_t s){
	SetMode(pos, true);
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

