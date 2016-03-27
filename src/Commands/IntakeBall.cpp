#include "IntakeBall.h"

#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

/**
 * Namespace commands with implementation
 * of IntakeBall command functions.
 */
namespace commands
{

using namespace dman;

// Constructor:
IntakeBall::IntakeBall(Intake * intake) : Command("IntakeBall")
{
	intake_ = intake;
	Requires(intake);
	SetInterruptible(true);
}

// Main functions:
void IntakeBall::Initialize()
{
	if (intake_->GetState() == State_t::OFF)
	{
		TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "IntakeBall", "Command")) <<
					"Initializing IntakeBall";
		intake_->SetState(State_t::TAKING);
		intake_->TakeBall(true);
	}
	else
	{
		TextLog::Log(MessageData(MessageData::ERR), SystemData("Intake", "IntakeBall", "Command")) <<
					"Failed to initialize IntakeBall : Invalid starting state (should be \"OFF\")";
		Cancel();
	}
}

bool IntakeBall::IsFinished()
{
	return intake_->CheckSwitch();
}

void IntakeBall::End()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "IntakeBall", "Command")) <<
				"Ending IntakeBall";
	intake_->Stop();
	intake_->SetState(State_t::HOLDING);
}

void IntakeBall::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "IntakeBall", "Command")) <<
				"Interrupting IntakeBall";
	intake_->Stop();
	if (intake_->CheckSwitch())
		intake_->SetState(State_t::HOLDING);
	else
		intake_->SetState(State_t::OFF);
}

} // end namespace commands
