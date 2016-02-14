#include "IntakeBall.h"

/**
 * Namespace commands with implementation
 * of IntakeBall command functions.
 */
namespace commands
{

// Constructor:
IntakeBall::IntakeBall(Intake * intake) : Command("IntakeBall")
{
	intake_ = intake;
	SetInterruptible(true);
}

// Main functions:
void IntakeBall::Initialize()
{
	if (intake_->GetState() == State_t::OFF)
	{
		intake_->SetState(State_t::TAKING);
		intake_->TakeBall(true);
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (Should be \"OFF\")";
	}
}

bool IntakeBall::IsFinished()
{
	return intake_->CheckSwitch();
}

void IntakeBall::End()
{
	intake_->Stop();
	intake_->SetState(State_t::HOLDING);
}

void IntakeBall::Interrupted()
{
	intake_->Stop();
	intake_->SetState(State_t::OFF);
}

} // end namespace commands
