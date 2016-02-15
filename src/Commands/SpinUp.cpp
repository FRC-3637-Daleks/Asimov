#include "SpinUp.h"

/**
 * Commands namespace with implementation
 * of SpinUp command functions.
 */
namespace commands
{

// Constructor:
SpinUp::SpinUp(Shooter *shooter, double speed) : Command("Spin Up")
{
	shooter_ = shooter;
	speed_ = speed;
	SetInterruptible(true);
}

// Main functions:
void SpinUp::Initialize()
{
	std::cout << "Shooter : SpinUp : Started";
	State_t currState = shooter_->GetState(); // For readability
	if (currState == State_t::OFF || currState == State_t::SPINNINGUP)
	{
		shooter_->SpinUp(speed_);
		if (currState != State_t::SPINNINGUP)
			shooter_->SetState(State_t::SPINNINGUP);
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (should be \"OFF\" or \"SPINNINGUP\")";
	}
}

bool SpinUp::IsFinished()
{
	return !shooter_->IsAllowable();
}

void SpinUp::End()
{
	std::cout << "Shooter : SpinUp : Ended";
	shooter_->SetState(State_t::SPUNUP);
}

void SpinUp::Interrupted()
{
	std::cout << "Shooter : SpinUp : Interrupted";
	shooter_->EmergencyStop();
	shooter_->SetState(State_t::OFF);
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
