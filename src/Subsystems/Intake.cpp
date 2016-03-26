#include "Intake.h"
#include "Commands/IntakeBall.h"
#include "Commands/PushBall.h"
#include <math.h>

/**
 * Subsystems namespace with implementation
 * of Intake class.
 */
namespace subsystems
{

using IntakeBall = commands::IntakeBall;
using PushBall = commands::PushBall;

// Constructor:
Intake::Intake() : dman::WPISystem("Intake")
{
	roller_ = NULL; // keep null until configure time
	detector_ = new DigitalInput(0);

	// Default values:
	intake_speed_ = 0.8;
	push_speed_ = -1.0;

	state_ = State_t::OFF;
	mode_ = Mode_t::VELOCITY;

	max_velocity_ = 30;
	allowed_error_ = 0.1;
	shoot_velocity_ = 1.0;
}

// Destructor:
Intake::~Intake()
{
	delete roller_;
	delete detector_;
}

// Dalek Manager config functions:
void Intake::doRegister()
{
	// CAN port settings:
	auto& can_ports = GetPortSpace("CAN");
	can_ports("intake_roller").SetDefault(5);

	// Instance variable settings
	auto& settings = GetSettings();
	settings("intake_speed").SetDefault(intake_speed_);
	settings("push_speed").SetDefault(push_speed_);
	settings("max_velocity").SetDefault(max_velocity_);
	settings("allowed_error").SetDefault(allowed_error_);
	settings("shoot_velocity").SetDefault(shoot_velocity_);

	// PID Settings | TODO: Set proper default values
	auto& closed_loop_settings = settings["closed_loop_settings"];
	closed_loop_settings("use").SetDefault(false);
	closed_loop_settings("P").SetDefault(.1);
	closed_loop_settings("I").SetDefault(0.0);
	closed_loop_settings("D").SetDefault(0.0);
	closed_loop_settings("F").SetDefault(1.0);
	closed_loop_settings("I_Zone").SetDefault(2.5);
	closed_loop_settings("ramp_rate").SetDefault(10);

	// Inversion settings
	settings["intake_roller"]("invert_output").SetDefault(false);
	settings["intake_roller"]("invert_sensor").SetDefault(false);
}

bool Intake::doConfigure()
{
	std::cout << "Configuring Intake" << std::endl; // TODO: Change cout to Log

	// Initialize roller to CAN port
	auto& can_ports = GetPortSpace("CAN");
	if (roller_ == NULL)
		roller_ = new CANTalon(can_ports("intake_roller").GetValueOrDefault());

	// Configure variable values
	auto& settings = GetSettings();
	SetIntakeSpeed(settings("intake_speed").GetValueOrDefault());
	SetPushSpeed(settings("push_speed").GetValueOrDefault());
	SetMaxVelocity(settings("max_velocity").GetValueOrDefault());
	SetAllowedError(settings("allowed_error").GetValueOrDefault());
	SetShootVelocity(settings("shoot_velocity").GetValueOrDefault());

	// Configure closed loop settings
	auto& closed_loop_settings = settings["closed_loop_settings"];
	if (closed_loop_settings("use").GetValueOrDefault() == true)
	{
		roller_->SetP(closed_loop_settings("P").GetValueOrDefault());
		roller_->SetI(closed_loop_settings("I").GetValueOrDefault());
		roller_->SetD(closed_loop_settings("D").GetValueOrDefault());
		roller_->SetF(closed_loop_settings("F").GetValueOrDefault());
		roller_->SetIzone(closed_loop_settings("I_Zone").GetValueOrDefault());
		roller_->SetCloseLoopRampRate(closed_loop_settings("ramp_rate").GetValueOrDefault());
	}

	// Configure roller settings
	roller_->SetInverted(settings["intake_roller"]("invert_output").GetValueOrDefault());
	roller_->SetSensorDirection(settings["intake_roller"]("invert_sensor").GetValueOrDefault());

	return true;
}

// Main functions:
void Intake::Initialize()
{
	roller_->SetFeedbackDevice(CANTalon::QuadEncoder);
	roller_->ConfigEncoderCodesPerRev(1024);
	roller_->SetSensorDirection(false);
	roller_->SetInverted(true);
	roller_->SelectProfileSlot(0);
	roller_->SetVoltageRampRate(0.0);
	roller_->SetCloseLoopRampRate(5.0);

	// Configure max and min voltage outputs
	roller_->ConfigNominalOutputVoltage(0.0, 0.0);
	roller_->ConfigPeakOutputVoltage(12.0, -12.0);
}

bool Intake::CheckSwitch() const
{
	return !(detector_->Get());
}

void Intake::TakeBall(bool check)
{
	if (CheckSwitch() && check)
		Stop();
	else
		SetSpeed(intake_speed_);
}

void Intake::OutakeBall()
{
	SetSpeed(push_speed_);
}

void Intake::ShootBall()
{
	SetSpeed(shoot_velocity_);
}

void Intake::SetSpeed(double speed)
{
	if (mode_ == Mode_t::VELOCITY)
	{
		roller_->Set(max_velocity_ * speed);
	}
	else if (mode_ == Mode_t::VBUS)
	{
		roller_->Set(speed);
	}
}

double Intake::GetSpeed() const
{
	return roller_->Get();
}

void Intake::Stop()
{
	SetSpeed(0.0);
}

void Intake::SetIntakeSpeed(double intake_speed)
{
	intake_speed_ = intake_speed;
}

void Intake::SetPushSpeed(double push_speed)
{
	push_speed_ = push_speed;
}

double Intake::GetIntakeSpeed() const
{
	return intake_speed_;
}

double Intake::GetPushSpeed() const
{
	return push_speed_;
}

double Intake::GetMaxVelocity() const
{
	return max_velocity_;
}

void Intake::SetMaxVelocity(double max_velocity)
{
	max_velocity_ = max_velocity;
}

void Intake::SetShootVelocity(double shoot_velocity)
{
	shoot_velocity_ = shoot_velocity;
}

double Intake::GetShootVelocity() const
{
	return shoot_velocity_;
}

// Error functions:
double Intake::GetErr() const
{
	return roller_->GetClosedLoopError();
}

void Intake::SetAllowedError(double err)
{
	allowed_error_ = err;
}

double Intake::GetAllowedError() const
{
	return allowed_error_;
}

bool Intake::IsAllowable() const
{
	return (fabs(GetErr()) > fabs(allowed_error_));
}

// State functions:
void Intake::SetState(State_t state)
{
	state_ = state;
}

Intake::State_t Intake::GetState() const
{
	return state_;
}

// Control mode functions:
void Intake::SetMode(Mode_t mode)
{
	if (mode_ != mode)
	{
		mode_ = mode;
		if (mode_ == Mode_t::VELOCITY)
			roller_->SetControlMode(CANTalon::ControlMode::kSpeed);
		else if (mode_ == Mode_t::VBUS)
			roller_->SetControlMode(CANTalon::ControlMode::kPercentVbus);
	}
}

Intake::Mode_t Intake::GetMode() const
{
	return mode_;
}

// Command functions:
IntakeBall* Intake::MakeIntakeBall()
{
	return new IntakeBall(this);
}

PushBall* Intake::MakePushBall()
{
	return new PushBall(this);
}

} // end namespace subsystems
