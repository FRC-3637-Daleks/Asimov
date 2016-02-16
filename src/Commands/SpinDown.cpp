#include "SpinDown.h"

/**
 * Commands namespace with implementation
 * of SpinDown class functions
 */
namespace commands
{

// Constructor:
SpinDown::SpinDown(Shooter* shooter) : Command("Spin Down")
{
	shooter_ = shooter;
	SetInterruptible(false);
}

// Main functions:
void SpinDown::Initialize()
{
	std::cout << "Shooter : SpinDown : Started" << std::endl;
	if (shooter_->GetState() != State_t::SHOOTING)  // Should only cancel if it was in the Shooting state
	{
		shooter_->SpinDown();
		shooter_->SetState(State_t::SPINNINGDOWN);
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (Should be \"SPUNUP\")" << std::endl;
		Cancel();
	}
}

bool SpinDown::IsFinished()
{
	return !shooter_->IsAllowable();
}

void SpinDown::End()
{
	std::cout << "Shooter : SpinDown : Ended" << std::endl;
	shooter_->SetState(State_t::OFF);
}

void SpinDown::Interrupted()
{
	std::cout << "Shooter : SpinDown : Interrupted" << std::endl;
	if(shooter_->GetState() != State_t::SHOOTING)
	{
		shooter_->EmergencyStop();
		shooter_->SetState(State_t::OFF);
	}
}

}// end namespace commands
