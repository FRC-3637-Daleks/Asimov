#include "SpinDown.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

/**
 * Commands namespace with implementation
 * of SpinDown class functions
 */
namespace commands
{

using namespace dman;

// Constructor:
SpinDown::SpinDown(Shooter* shooter) : Command("Spin Down")
{
	shooter_ = shooter;
	SetInterruptible(false);
	Requires(shooter);
}

// Main functions:
void SpinDown::Initialize()
{
	if (shooter_->GetState() != State_t::SHOOTING)  // Should only cancel if it was in the Shooting state
	{
		TextLog::Log(MessageData(MessageData::INFO), SystemData("Shooter", "SpinDown", "Command")) <<
				"Initializing SpinDown";

		shooter_->SpinDown();
		shooter_->SetState(State_t::SPINNINGDOWN);
	}
	else
	{
		TextLog::Log(MessageData(MessageData::ERR), SystemData("Shooter", "SpinDown", "Command")) <<
					"Failed to initialize SpinDown : Invalid starting state (Should not be \"SHOOTING\")";
		Cancel();
	}
}

bool SpinDown::IsFinished()
{
	return !shooter_->IsAllowable();
}

void SpinDown::End()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Shooter", "SpinDown", "Command")) <<
				"Ending SpinDown";
	shooter_->SetState(State_t::OFF);
}

void SpinDown::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Shooter", "SpinDown", "Command")) <<
				"Interrupting SpinDown";
	if(shooter_->GetState() != State_t::SHOOTING)
	{
		shooter_->EmergencyStop();
		shooter_->SetState(State_t::OFF);
	}
}

}// end namespace commands
