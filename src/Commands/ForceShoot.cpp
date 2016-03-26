#include "ForceShoot.h"

/**
 * Commands namespace with implementation
 * of ForceShoot command.
 */
namespace commands
{

// Constructor:
ForceShoot::ForceShoot(Intake *intake) : Command("ForceShoot")
{
	intake_ = intake;
	Requires(intake);
	SetInterruptible(true);
}

// Main functions:
void ForceShoot::Initialize()
{
	std::cout << "Intake : ForceShoot : Started" << std::endl;
	intake_->ShootBall();
}

bool ForceShoot::IsFinished()
{
	return false;
}

void ForceShoot::Interrupted()
{
	std::cout << "Intake : ForceShoot : Interrupted" << std::endl;
	intake_->Stop();
}

} // end namespace commands
