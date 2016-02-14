#include "Shoot.h"

/**
 * Commands namespace with implementation
 * of Shoot command.
 */
namespace commands
{

//Constructor:
Shoot::Shoot(Intake* intake, Shooter* shooter)
{
	intake_ = intake;
	shooter_ = shooter;
}

void Shoot::Initialize()
{
	if (shooter_->GetState() == State_t::SPUNUP)
	{
		shooter_->SetState(State_t::SHOOTING);
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (should be \"SPUNUP\")";
	}
}

void Shoot::End()
{
	shooter_->SetState(State_t::SPUNUP);
}

} // end namespace commands
