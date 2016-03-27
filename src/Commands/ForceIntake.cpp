#include "ForceIntake.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

/**
 * Commands namespace with implementation
 * of ForceIntake command.
 */
namespace commands
{

using namespace dman;

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
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "ForceIntake", "Command")) <<
				"Initializing ForceIntake";
	intake_->TakeBall(false);
}

bool ForceIntake::IsFinished()
{
	return false;
}

void ForceIntake::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "ForceIntake", "Command")) <<
				"Interrupting ForceIntake";
	intake_->Stop();
}

} // end namespace commands


