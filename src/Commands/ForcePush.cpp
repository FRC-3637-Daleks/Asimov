#include "ForcePush.h"

/**
 * Commands namespace with implementation
 * of ForcePush command.
 */
namespace commands
{

// Constructor:
ForcePush::ForcePush(Intake *intake) : Command("ForcePush")
{
	intake_ = intake;
	Requires(intake);
	SetInterruptible(true);
}

// Main functions:
void ForcePush::Initialize()
{
	std::cout << "Intake : ForcePush : Started" << std::endl;
	intake_->OutakeBall();
}

bool ForcePush::IsFinished()
{
	return false;
}

void ForcePush::Interrupted()
{
	std::cout << "Intake : ForcePush : Interrupted" << std::endl;
	intake_->Stop();
}

} // end namespace commands
