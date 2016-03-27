#include "ForcePush.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

/**
 * Commands namespace with implementation
 * of ForcePush command.
 */
namespace commands
{

using namespace dman;

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
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "ForcePush", "Command")) <<
				"Initializing ForcePush";
	intake_->OutakeBall();
}

bool ForcePush::IsFinished()
{
	return false;
}

void ForcePush::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "ForcePush", "Command")) <<
				"Interrupting ForcePush";
	intake_->Stop();
}

} // end namespace commands
