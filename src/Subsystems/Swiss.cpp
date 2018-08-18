#include "WPILib.h"

#include "Swiss.h"
#include "ctre/Phoenix.h"
#include "Commands/SetSwiss.h"
#include "Commands/HoldSwiss.h"
#include "Commands/ControlSwissVelocity.h"

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
	settings("closed_loop_output").SetDefault(false);
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

	GetLocalValue<double>("current_position").Initialize(std::make_shared<FunkyGet<double> >([this]() -> double {
					return GetPos();
				}));
		GetLocalValue<double>("target_position").Initialize(std::make_shared<FunkyGet<double> >([this]() -> double {
						if(swisstalon)
							return (double)swisstalon->GetClosedLoopTarget(0);
						return 0.0;
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
		GetLocalValue<double>("talon/output_voltage").Initialize(std::make_shared<FunkyGet<double> >([this]() -> double {
					if(swisstalon)
						return swisstalon->GetMotorOutputVoltage();
					return 0.0;
				}));
		GetLocalValue<double>("talon/output_speed").Initialize(std::make_shared<FunkyGet<double> >([this]() -> double {
					if(swisstalon)
						return swisstalon->GetSensorCollection().GetQuadratureVelocity();
					return 0.0;
		}));
		GetLocalValue<double>("talon/output_current").Initialize(std::make_shared<FunkyGet<double> >([this]() -> double {
					if(swisstalon)
						return swisstalon->GetOutputCurrent();
					return 0.0;
				}));
		GetLocalValue<double>("talon/temperature").Initialize(std::make_shared<FunkyGet<double> >([this]() -> double {
					if(swisstalon)
						return swisstalon->GetTemperature();
					return 0.0;
				}));
		GetLocalValue<double>("talon/input_voltage").Initialize(std::make_shared<FunkyGet<double> >([this]() -> double {
					if(swisstalon)
						return swisstalon->GetBusVoltage();
					return 0.0;
				}));
}

bool Swiss::doConfigure()
{
	initTalon();
	Log(MessageData(MessageData::INFO, 2), "", "") << "Talon initialized";
	auto& settings = GetSettings();

	{
		auto& positions = settings["positions"];
		for(int i = 0; i < n_states; i++)
			states[i] = positions(StateToString(static_cast<state_t>(i))).GetValueOrDefault<double>();
	}
	swisstalon->ConfigSelectedFeedbackSensor(FeedbackDevice::Analog, 0, 10);
	swisstalon->SetNeutralMode(NeutralMode::Brake);

	double reverse_limit = 1000000;
	double forward_limit = -1000000;
	for(auto state : states)
	{
		if(state < reverse_limit) reverse_limit = state;
		if(state > forward_limit) forward_limit = state;
	}
	Log(MessageData(MessageData::INFO, 3), "", "") << "Reverse Limit: " << reverse_limit <<
			", forward_limit" << forward_limit;
	swisstalon->ConfigReverseSoftLimitEnable(reverse_limit, 10);
	swisstalon->ConfigForwardSoftLimitEnable(forward_limit, 10);
	swisstalon->ConfigOpenloopRamp(settings("ramp_rate").GetValueOrDefault<double>(), 10);

	swisstalon->SetSensorPhase(settings("sensor_flipped").GetValueOrDefault<bool>());
	swisstalon->SetInverted(settings("output_flipped").GetValueOrDefault<bool>());

	// idk
	swisstalon->ConfigNominalOutputForward(0.0, 10);
	swisstalon->ConfigNominalOutputReverse(0.0, 10);
	swisstalon->ConfigPeakOutputForward(1.0, 10);
	swisstalon->ConfigPeakOutputReverse(-1.0, 10);

	if(settings["closed_loop"]("use").GetValueOrDefault<bool>()) {
		Log(MessageData(MessageData::INFO, 2), "", "") << "Using file PID";
		auto& closed_loop = settings["closed_loop"];
		pPid.p = closed_loop("P").GetValueOrDefault<double>();
		pPid.i = closed_loop("I").GetValueOrDefault<double>();
		pPid.d = closed_loop("D").GetValueOrDefault<double>();
		pPid.f = closed_loop("F").GetValueOrDefault<double>();
		pPid.izone = closed_loop("I-Zone").GetValueOrDefault<int>();
		swisstalon->SelectProfileSlot(0, 0);
		swisstalon->Config_kP(0, pPid.p, 10);
		swisstalon->Config_kI(0, pPid.i, 10);
		swisstalon->Config_kD(0, pPid.d, 10);
		swisstalon->Config_kF(0, pPid.f, 10);
		swisstalon->Config_IntegralZone(0, pPid.izone, 10);
		swisstalon->ConfigClosedloopRamp(settings("ramp_rate").GetValueOrDefault<double>(), 10);
	}
	else
	{
		Log(MessageData(MessageData::INFO, 2), "", "") << "Using talon PID";
	}

		SetMode(pos);
	return true;
}

void Swiss::initTalon()
{
	if(is_initialized())
		return;

	swisstalon = new WPI_TalonSRX(GetPortSpace("CAN")("talon").GetValueOrDefault());
}

Swiss::mode_t Swiss::GetMode() const {
	return static_cast<mode_t>(swisstalon->GetControlMode());
}

void Swiss::SetMode(mode_t m) {
	if(GetMode() == m){
		return;

	}
	if(m == pos){
		swisstalon->SelectProfileSlot(0, 0);
		swisstalon->Set(ControlMode::Position, swisstalon->Get());
		return;
	}
	else if(m == velocity){
		swisstalon->SelectProfileSlot(1, 0);
		SetVelocity(0.0, false);
		return;
	}
	else{
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
		swisstalon->Set(ControlMode::Velocity, maxVelocity * v);
	else if(GetMode() == mode_t::vbus)
		swisstalon->Set(ControlMode::PercentOutput, v);
}

double Swiss::GetPos() const {
	if(swisstalon)
		return (double)swisstalon->GetSensorCollection().GetAnalogIn();
	else
		return 0.0;
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
	SetMode(vbus);
	SetVoltage(0.0);
}

void Swiss::SetAllowableError(double err) {
	allowable_error = err;
	// swisstalon->SetAllowableClosedLoopErr(allowable_error);
}

commands::SetSwiss *Swiss::MakeSetSwiss(state_t state)
{
	return new commands::SetSwiss(this, state);
}

commands::ControlSwissVelocity *Swiss::MakeControlSwissVelocity(ValueStore::Value<double> input)
{
	return new commands::ControlSwissVelocity(this, std::move(input));
}

commands::HoldSwiss *Swiss::MakeHoldSwiss()
{
	return new commands::HoldSwiss(this);
}

}
