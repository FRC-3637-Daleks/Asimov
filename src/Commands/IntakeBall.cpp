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
	std::cout << "Intake : IntakeBall : Started" << std::endl;
	if (intake_->GetState() == State_t::OFF)
	{
		intake_->SetState(State_t::TAKING);
		intake_->TakeBall(true);
	}
	else
	{
		std::cout << "ERROR: Invalid starting state (Should be \"OFF\")" << std::endl;
		Cancel();
	}
}

bool IntakeBall::IsFinished()
{
	return intake_->CheckSwitch();
}

void IntakeBall::End()
{
	std::cout << "Intake : IntakeBall : Ended" << std::endl;
	intake_->Stop();
	intake_->SetState(State_t::HOLDING);
}

void IntakeBall::Interrupted()
{
	std::cout << "Intake : IntakeBall : Interrupted " << std::endl;
	intake_->Stop();
	if (intake_->CheckSwitch())
		intake_->SetState(State_t::HOLDING);
	else
		intake_->SetState(State_t::OFF);
}

} // end namespace commands
