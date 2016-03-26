#include "ForceShoot.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

/**
 * Commands namespace with implementation
 * of ForceShoot command.
 */
namespace commands
{

using namespace dman;

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
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Shooter", "ForceShoot", "Command")) <<
				"Initializing ForceShoot";
	intake_->ShootBall();
}

bool ForceShoot::IsFinished()
{
	return false;
}

void ForceShoot::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Shooter", "ForceShoot", "Command")) <<
				"Interrupting ForceShoot";
	intake_->Stop();
}

} // end namespace commands
