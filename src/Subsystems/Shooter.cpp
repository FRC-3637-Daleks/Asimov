#include "Shooter.h"
#include "Commands/SpinUp.h"
#include "Commands/SpinDown.h"
#include <math.h>

using Shooter = subsystems::Shooter;
using SpinUp = commands::SpinUp;
using SpinDown = commands::SpinDown;

Shooter::Shooter() : Subsystem("Shooter")
{
	top_roller_ = new CANTalon(1);
	allowed_error_ = 0.1;
	state_ = State_t::OFF;
	mode_ = Mode_t::VELOCITY;
	max_velocity_ = 30;
}

Shooter::~Shooter()
{
	delete top_roller_;
}

void Shooter::Initialize()
{
	top_roller_->SetFeedbackDevice(CANTalon::QuadEncoder);
	top_roller_->ConfigEncoderCodesPerRev(1024);
	top_roller_->SetSensorDirection(false);
	top_roller_->SelectProfileSlot(0);
	top_roller_->SetVoltageRampRate(0.0);
	top_roller_->SetCloseLoopRampRate(0.0);

	// Set max and min voltage ouput, dissalows negative voltage
	top_roller_->ConfigNominalOutputVoltage(0.0, 0.0);
	top_roller_->ConfigPeakOutputVoltage(12.0, 0.0);
}

void Shooter::SpinUp(double speed)
{
	double target = top_roller_->Get() * speed;
	top_roller_->Set(target);

	///////////////////HALP////////////////////////

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

double Shooter::GetErr() const
{
	return top_roller_->GetSetpoint() - top_roller_->Get();
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


// States and modes or whatever:
Shooter::State_t Shooter::GetState() const
{
	return state_;
}

void Shooter::SetState(State_t state)
{
	state_ = state;
}

double Shooter::GetMaxVelocity() const
{
	return max_velocity_;
}

void Shooter::SetMaxVelocity(double max_velocity)
{
	max_velocity_ = max_velocity;
}

// Command functions:
SpinUp* Shooter::MakeSpinUp()
{
	return new commands::SpinUp(this, 1.0);
}

SpinDown* Shooter::MakeSpinDown()
{
	return new commands::SpinDown(this);
}
