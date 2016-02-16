#include "SpinUp.h"

/**
 * Commands namespace with implementation
 * of SpinUp command functions.
 */
namespace commands
{

// Constructor:
SpinUp::SpinUp(Shooter *shooter, double speed, double wait_time) : Command("Spin Up")
{
	shooter_ = shooter;
	speed_ = speed;
	wait_time_ = wait_time;
	timer_ = new Timer;
	Requires(shooter);
	SetInterruptible(true);
}

// Main functions:
void SpinUp::Initialize()
{
	std::cout << "Shooter : SpinUp : Started with speed = " << speed_ << " and wait time = " << wait_time_ << std::endl;
	State_t currState = shooter_->GetState(); // For readability
	if (currState == State_t::OFF || currState == State_t::SPINNINGUP || currState == State_t::SPUNUP)
	{
		timer_->Reset();
		timer_->Start();
		shooter_->SpinUp(speed_);
		if (currState != State_t::SPINNINGUP)
			shooter_->SetState(State_t::SPINNINGUP);
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (should be \"OFF\" or \"SPINNINGUP\")" << std::endl;
		Cancel();
	}
}

bool SpinUp::IsFinished()
{
	return (!shooter_->IsAllowable()) && (timer_->Get() > wait_time_);
}

void SpinUp::End()
{
	std::cout << "Shooter : SpinUp : Ended" << std::endl;
	shooter_->SetState(State_t::SPUNUP);
}

void SpinUp::Interrupted()
{
	std::cout << "Shooter : SpinUp : Interrupted" << std::endl;
	//shooter_->EmergencyStop();
	//shooter_->SetState(State_t::OFF);
}

void SpinUp::SetSpeed(double speed)
{
	speed_ = speed;
}

double SpinUp::GetSpeed() const
{
	return speed_;
}

}// end namespace commands
