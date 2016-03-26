#include "Commands/ShootIntake.h"

/**
 * Commands namespace with implementation
 * of ShootIntake command.
 */
namespace commands
{

// Constructor:
ShootIntake::ShootIntake(Intake *intake, Shooter *shooter, double shoot_time, double timeout) : Command("ShootIntake")
{
	intake_ = intake;
	shooter_ = shooter;

	timeout_ = timeout;
	shoot_time_ = shoot_time;
	check_switch_ = true;

	timer_ = new Timer;

	Requires(intake);
	SetTimeout(timeout_);
	SetInterruptible(false);
}

// Main functions:
void ShootIntake::Initialize()
{
	std::cout << "Intake and Shooter : ShootIntake : Started with shoot time = " << shoot_time_ << " and timeout = " << timeout_ << std::endl;
	if (shooter_->GetState() == Shooter::State_t::SPUNUP && intake_->GetState() == Intake::State_t::HOLDING)
	{
		timer_->Reset();
		shooter_->SetState(Shooter::State_t::SHOOTING);
		intake_->SetState(Intake::State_t::SHOOTING);
		intake_->ShootBall();
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (should be \"SPUNUP\" and \"HOLDING\")" << std::endl;
		Cancel();
	}
}

void ShootIntake::Execute()
{
	if (check_switch_ && !intake_->CheckSwitch())
	{
		timer_->Start();
		check_switch_ = false;
	}
}

void ShootIntake::End()
{
	std::cout << "Intake and Shooter : ShootIntake : Ended" << std::endl;

	if(shooter_->GetState() == Shooter::State_t::SHOOTING)
		shooter_->SetState(Shooter::State_t::SPUNUP);
	intake_->SetState(Intake::State_t::OFF);
	intake_->Stop();
}

bool ShootIntake::IsFinished()
{
	return IsTimedOut() || timer_->Get() >= shoot_time_;
}

void ShootIntake::Interrupted()
{
	std::cout << "Intake and Shooter : ShootIntake : Interrupted" << std::endl;
	if(intake_->GetState() == Intake::State_t::SHOOTING)  // Prevents shooter from ending in wrong state after cancel
		End();
}

void ShootIntake::SetShootTime(double shoot_time)
{
	shoot_time_ = shoot_time;
}

double ShootIntake::GetShootTime() const
{
	return shoot_time_;
}

void ShootIntake::SetOverallTimeout(double time_out)
{
	timeout_ = time_out;
	SetTimeout(timeout_);
}

double ShootIntake::GetOverallTimeout() const
{
	return timeout_;
}

} // end namespace commands
