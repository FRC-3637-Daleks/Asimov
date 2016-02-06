#include "Shooter.h"
//include commands/(shooting command)

using Shooter = subsystems::Shooter;

Shooter::Shooter() : Subsystem("Shooter")
{
	top_roller_ = new CANTalon(1);
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
}

void Shooter::SpinDown()
{
//	top_roller_->Set(0.0);
}
