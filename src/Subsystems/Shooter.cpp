#include "Shooter.h"
#include "Commands/SpinUp.h"
#include "Commands/SpinDown.h"
#include <math.h>

namespace subsystems
{

using SpinUp = commands::SpinUp;
using SpinDown = commands::SpinDown;

// Constructor:
Shooter::Shooter() : dman::WPISystem("Shooter")
{
	top_roller_ = NULL; // leave null until configuration

	state_ = State_t::OFF;
	mode_ = Mode_t::VBUS;
	SetMode(Mode_t::VELOCITY);

	max_velocity_ = 2400000;
	allowed_error_ = 0.1;
}

// Destructor:
Shooter::~Shooter()
{
	delete top_roller_;
}

// Dalek manager config functions:
void Shooter::doRegister()
{
	// CAN port settings
	auto& can_ports = GetPortSpace("CAN");
	can_ports("shooter_roller").SetDefault(6);

	// Instance variable settings
	auto& settings = GetSettings();
	settings("max_velocity").SetDefault(max_velocity_);
	settings("allowed_error").SetDefault(allowed_error_);

	// PID settings
	auto& closed_loop_settings = settings["closed_loop_settings"];
	closed_loop_settings("use").SetDefault(false);
	closed_loop_settings("P").SetDefault(.1);
	closed_loop_settings("I").SetDefault(0.0);
	closed_loop_settings("D").SetDefault(0.0);
	closed_loop_settings("F").SetDefault(1.0);
	closed_loop_settings("I_Zone").SetDefault(2.5);
	closed_loop_settings("ramp_rate").SetDefault(10);

	// Inversion settings
	settings["shooter_roller"]("invert_output").SetDefault(false);
	settings["shooter_roller"]("invert_sensor").SetDefault(false);
}

bool Shooter::doConfigure()
{
	Log(dman::MessageData::INFO, "", "Subsystem") << "Configuring Shooter";

	// Initialize roller to CAN port
	auto& can_ports = GetPortSpace("CAN");
	if (top_roller_ == NULL)
		top_roller_ = new CANTalon(can_ports("shooter_roller").GetValueOrDefault());

	// Configure variable values
	auto& settings = GetSettings();
	SetMaxVelocity(settings("max_velocity").GetValueOrDefault());
	SetAllowedError(settings("allowed_error").GetValueOrDefault());

	// Configure closed loop settings
	auto& closed_loop_settings = settings["closed_loop_settings"];
	if (closed_loop_settings("use").GetValueOrDefault() == true)
	{
		top_roller_->SetP(closed_loop_settings("P").GetValueOrDefault());
		top_roller_->SetI(closed_loop_settings("I").GetValueOrDefault());
		top_roller_->SetD(closed_loop_settings("D").GetValueOrDefault());
		top_roller_->SetF(closed_loop_settings("F").GetValueOrDefault());
		top_roller_->SetIzone(closed_loop_settings("I_Zone").GetValueOrDefault());
		top_roller_->SetCloseLoopRampRate(closed_loop_settings("ramp_rate").GetValueOrDefault());
	}

	// Configure roller settings
	top_roller_->SetInverted(settings["intake_roller"]("invert_output").GetValueOrDefault());
	top_roller_->SetSensorDirection(settings["intake_roller"]("invert_sensor").GetValueOrDefault());

	return true;
}

// Main functions:
void Shooter::Initialize()
{
	top_roller_->SetFeedbackDevice(CANTalon::QuadEncoder);
	top_roller_->ConfigEncoderCodesPerRev(2);
	top_roller_->SetInverted(true);
	top_roller_->SetClosedLoopOutputDirection(false);
	top_roller_->SetSensorDirection(true);
	top_roller_->SelectProfileSlot(0);
	top_roller_->SetVoltageRampRate(0.0);
	//top_roller_->SetCloseLoopRampRate(0.0);

	// Set max and min voltage ouput, dissalows negative voltage
	top_roller_->ConfigNominalOutputVoltage(0.0, 0.0);
	top_roller_->ConfigPeakOutputVoltage(0, -12.0);
}

void Shooter::SpinUp(double speed)
{
	if (mode_ == Mode_t::VELOCITY)
	{
		top_roller_->Set(max_velocity_ * speed);
	}
	else if (mode_ == Mode_t::VBUS)
	{
		top_roller_->Set(speed);
	}
}

void Shooter::SpinDown()
{
	top_roller_->Set(0.0);
}

void Shooter::EmergencyStop()
{
	top_roller_->StopMotor();
}

double Shooter::GetMaxVelocity() const
{
	return max_velocity_;
}

void Shooter::SetMaxVelocity(double max_velocity)
{
	max_velocity_ = max_velocity;
}

double Shooter::GetSpeed() const
{
	return top_roller_->GetSpeed();
}

// Error functions:
double Shooter::GetErr() const
{
	return top_roller_->GetClosedLoopError();
}

void Shooter::SetAllowedError(double err)
{
	allowed_error_ = err;
}

double Shooter::GetAllowedError() const
{
	return allowed_error_;
}

bool Shooter::IsAllowable() const
{
	return (fabs(GetErr()) > fabs(allowed_error_));
}

// State functions:
Shooter::State_t Shooter::GetState() const
{
	return state_;
}

void Shooter::SetState(State_t state)
{
	state_ = state;
}

// Control mode functions:
Shooter::Mode_t Shooter::GetMode() const
{
	return mode_;
}

void Shooter::SetMode(Mode_t mode)
{
	if (mode_ != mode)
	{
		mode_ = mode;
		if (mode_ == Mode_t::VELOCITY)
			top_roller_->SetControlMode(CANTalon::ControlMode::kSpeed);
		else if (mode_ == Mode_t::VBUS)
			top_roller_->SetControlMode(CANTalon::ControlMode::kPercentVbus);
	}
}

// Command functions:
SpinUp* Shooter::MakeSpinUp(double speed)
{
	return new commands::SpinUp(this, speed);
}

SpinDown* Shooter::MakeSpinDown()
{
	return new commands::SpinDown(this);
}

} // end namespace subsystems
