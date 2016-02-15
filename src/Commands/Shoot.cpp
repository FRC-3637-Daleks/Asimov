#include "Shoot.h"

/**
 * Commands namespace with implementation
 * of Shoot command.
 */
namespace commands
{

//Constructor:
Shoot::Shoot(Intake* intake, Shooter* shooter, double shoot_time)
{
	intake_ = intake;
	shooter_ = shooter;
	shoot_time_ = shoot_time;
	timer_ = new Timer;
	SetInterruptible(false);
}

void Shoot::Initialize()
{
	std::cout << "Intake and Shooter : Shoot : Started" << std::endl;
	if (shooter_->GetState() == State_t::SPUNUP)
	{
		timer_->Start();
		shooter_->SetState(Shooter::State_t::SHOOTING);
		intake_->SetState(Intake::State_t::SHOOTING);
		intake_->ShootBall();
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (should be \"SPUNUP\")";
	}
}

void Shoot::End()
{
	std::cout << "Intake and Shooter : Shoot : Ended" << std::endl;
	shooter_->SetState(Shooter::State_t::SPUNUP);
	intake_->SetState(Intake::State_t::OFF);
	intake_->Stop();
}

bool Shoot::IsFinished()
{
	return (timer_->Get() >= shoot_time_);
}

void Shoot::Interrupted()
{
	std::cout << "Intake and Shooter : Shoot : Interrupted" << std::endl;
	End();
}

void Shoot::SetShootTime(double shoot_time)
{
	shoot_time_ = shoot_time;
}

double Shoot::GetShootTime() const
{
	return shoot_time_;
}

} // end namespace commands
