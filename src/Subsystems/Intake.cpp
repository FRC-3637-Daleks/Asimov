#include "Intake.h"
#include "Commands/IntakeBall.h"

using Intake = subsystems::Intake;
using IntakeBall = commands::IntakeBall;

// Constructor:
Intake::Intake() : Subsystem("Intake")
{
	roller_ = new CANTalon(1);
	detector_ = new DigitalInput(2);
	intake_speed_ = 0.5; // temporary value
	push_speed_ = -0.7; // temporary value
}

// Destructor:
Intake::~Intake()
{
	delete roller_;
	delete detector_;
}

bool Intake::CheckSwitch() const
{
	return (detector_->Get());
}

void Intake::TakeBall(bool check)
{
	if (CheckSwitch() && check)
		Stop();
	else
		SetSpeed(intake_speed_);
}

void Intake::PushBall()
{
	SetSpeed(push_speed_);
}

void Intake::SetSpeed(double speed)
{
	if (speed > -1.0 && speed < 1.0)
		roller_->Set(speed);
}

double Intake::GetSpeed() const
{
	return roller_->Get();
}

void Intake::Stop()
{
	SetSpeed(0.0);
}

IntakeBall* Intake::MakeIntakeBall()
{
	return new IntakeBall(this);
}
