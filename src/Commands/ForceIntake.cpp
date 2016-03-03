#include "ForceIntake.h"

/**
 * Commands namespace with implementation
 * of ForceIntake command.
 */
namespace commands
{

// Constructor:
ForceIntake::ForceIntake(Intake *intake) : Command("ForceIntake")
{
	intake_ = intake;
	Requires(intake);
	SetInterruptible(true);
}

// Main functions:
void ForceIntake::Initialize()
{
	std::cout << "Intake : ForceIntake : Started" << std::endl;
	intake_->TakeBall(false);
}

bool ForceIntake::IsFinished()
{
	return false;
}

void ForceIntake::Interrupted()
{
	std::cout << "Intake : ForceIntake : Interrupted" << std::endl;
	intake_->Stop();
}

} // end namespace commands


