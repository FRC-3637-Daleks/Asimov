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
}

// Main functions:
void SpinDown::Initialize()
{
	if (shooter_->GetState() == State_t::SPUNUP)
	{
		shooter_->SpinDown();
		shooter_->SetState(State_t::SPINNINGDOWN);
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (Should be \"SPUNUP\")";
	}
}

bool SpinDown::IsFinished()
{
	return !shooter_->IsAllowable();
}

void SpinDown::End()
{
	shooter_->SetState(State_t::OFF);
}

}// end namespace commands
