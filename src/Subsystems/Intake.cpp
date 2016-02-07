#include "Intake.h"
#include "Commands/IntakeBall.h"
#include "Commands/PushBall.h"

using Intake = subsystems::Intake;
using IntakeBall = commands::IntakeBall;
using PushBall = commands::PushBall;

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

void Intake::OutakeBall()
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

IntakeBall* Intake::MakeIntakeBall()
{
	return new IntakeBall(this);
}

PushBall* Intake::MakePushBall()
{
	return new PushBall(this);
}
