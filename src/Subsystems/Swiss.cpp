#include "WPILib.h"

#include "Swiss.h"

#include "Utility/ValueStore.h"
#include "Utility/FunkyGet.h"

#include <cmath>
#include <string>

namespace subsystems
{

using namespace dman;

double Swiss::tickToDegree = 90; //measure and change later
double Swiss::maxVelocity = 18;
double Swiss::allowable_error = 5;



double Swiss::states[] = {
		/*[retract] =*/ -.032,
		/*[horizontal] =*/ -.63,
		/*[cheval_down] =*/ -.7,
		/*[port_down] =*/ -.780
};

std::string Swiss::StateToString(state_t state)
{
	static const char * const state_strings[] = {"retract", "horizontal", "cheval_down", "port_down"};
	if(state == n_states)
		return "";
	return state_strings[(int)state];
}


Swiss::Swiss() : WPISystem("Swiss"){
	current = retract;
	position = retract;
	swisstalon = nullptr;
}

void Swiss::doRegister()
{
	auto& ports = GetPortSpace("CAN");
	auto& settings = GetSettings();

	ports("talon");

	{
		auto& positions = settings["positions"];
		for(int i = 0; i < n_states; i++)
			positions(StateToString(static_cast<state_t>(i))).SetDefault(states[i]);
	}

	settings("pot_turns").SetDefault(1.0);
	settings("sensor_flipped").SetDefault(true);
	settings("output_flipped").SetDefault(true);
	settings("ramp_rate").SetDefault(10.0);

	{
		auto& closed_loop = settings["closed_loop"];
		closed_loop("use").SetDefault(false);
		closed_loop("P").SetDefault(10.0);
		closed_loop("I").SetDefault(0.001);
		closed_loop("D").SetDefault(0.0);
		closed_loop("F").SetDefault(0.0);
		closed_loop("I-Zone").SetDefault(50);
		closed_loop("allowable_error").SetDefault(get_allowable_error());
	}
}

bool Swiss::doConfigure()
{
	initTalon();
	auto& settings = GetSettings();

	{
		auto& positions = settings["positions"];
		for(int i = 0; i < n_states; i++)
			states[i] = positions(StateToString(static_cast<state_t>(i))).GetValueOrDefault<double>();
	}

	swisstalon->SetFeedbackDevice(CANTalon::FeedbackDevice::AnalogPot);
	swisstalon->ConfigPotentiometerTurns(settings("pot_turns").GetValueOrDefault<double>());
	swisstalon->ConfigNeutralMode(CANSpeedController::kNeutralMode_Brake);
	swisstalon->ConfigReverseLimit(states[port_down]);
	swisstalon->ConfigForwardLimit(states[retract]);
	swisstalon->ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SoftPositionLimits);
	swisstalon->SetVoltageRampRate(settings("ramp_rate").GetValueOrDefault<double>());

	swisstalon->SetSensorDirection(settings("sensor_flipped").GetValueOrDefault<bool>());
	swisstalon->SetClosedLoopOutputDirection(false);
	swisstalon->SetInverted(settings("output_flipped").GetValueOrDefault<bool>());

	if(settings["closed_loop"]("use").GetValueOrDefault<bool>()) {
		auto& closed_loop = settings["closed_loop"];
		pPid.p = closed_loop("P").GetValueOrDefault<double>();
		pPid.i = closed_loop("I").GetValueOrDefault<double>();
		pPid.d = closed_loop("D").GetValueOrDefault<double>();
		pPid.f = closed_loop("F").GetValueOrDefault<double>();
		pPid.izone = closed_loop("I-Zone").GetValueOrDefault<int>();
		swisstalon->SelectProfileSlot(0);
		swisstalon->SetPID(pPid.p, pPid.i, pPid.d, pPid.f);
		swisstalon->SetIzone(pPid.izone);
		SetAllowableError(settings("allowable_error").GetValueOrDefault<double>());
		swisstalon->SetCloseLoopRampRate(settings("ramp_rate").GetValueOrDefault<double>());
	}

	GetLocalValue<double>("current_position").Initialize(std::make_shared<FunkyGet<double> >([this]() {
				return GetPos();
			}));
	GetLocalValue<double>("target_position").Initialize(std::make_shared<FunkyGet<double> >([this]() {
					return swisstalon->GetSetpoint();
				}));
	GetLocalValue<state_t>("n_current_state").Initialize(std::make_shared<FunkyGet<state_t> >([this]() {
				return current;
			}));
	GetLocalValue<state_t>("n_target_state").Initialize(std::make_shared<FunkyGet<state_t> >([this]() {
					return position;
				}));
	GetLocalValue<std::string>("current_state").Initialize(std::make_shared<FunkyGet<std::string> >([this]() {
				return StateToString(current);
			}));
	GetLocalValue<std::string>("target_state").Initialize(std::make_shared<FunkyGet<std::string> >([this]() {
				return StateToString(position);
			}));
	GetLocalValue<double>("talon/output_voltage").Initialize(std::make_shared<FunkyGet<double> >([this]() {
				return swisstalon->GetOutputVoltage();
			}));
	GetLocalValue<double>("talon/output_current").Initialize(std::make_shared<FunkyGet<double> >([this]() {
				return swisstalon->GetOutputCurrent();
			}));
	GetLocalValue<double>("talon/temperature").Initialize(std::make_shared<FunkyGet<double> >([this]() {
				return swisstalon->GetTemperature();
			}));
	GetLocalValue<double>("talon/input_voltage").Initialize(std::make_shared<FunkyGet<double> >([this]() {
				return swisstalon->GetBusVoltage();
			}));

	SetMode(pos);
	return true;
}

void Swiss::initTalon()
{
	if(is_initialized())
		return;

	swisstalon = new CANTalon(GetPortSpace("CAN")("talon").GetValueOrDefault());
}

Swiss::mode_t Swiss::GetMode() const {
	return static_cast<mode_t>(swisstalon->GetControlMode());
}

void Swiss::SetMode(mode_t m) {
	if(GetMode() == m){
		return;

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

Swiss::state_t Swiss::GetState() const {
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

double Swiss::GetPos() const {
	return swisstalon->GetPosition();
}

double Swiss::GetDiff() const {
	if(GetMode()== Swiss::mode_t::pos){
		return GetState()-GetPos();
	}
	else{
		return 0;
	}


}
void Swiss::RefreshState(){
	if (IsCloseNuff()){
		current = position;
		return;
	}
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

bool Swiss::IsCloseNuff() const {
	return fabs(GetDiff()) <= get_allowable_error();
}
void Swiss::Hold(){
	swisstalon->Set(GetPos());
}

void Swiss::SetAllowableError(double err) {
	allowable_error = err;
	swisstalon->SetAllowableClosedLoopErr(allowable_error);
}

}
