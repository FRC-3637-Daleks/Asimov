#include "SpinUp.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

/**
 * Commands namespace with implementation
 * of SpinUp command functions.
 */
namespace commands
{

using namespace dman;

// Constructor:
SpinUp::SpinUp(Shooter *shooter, double speed, double wait_time) : Command("Spin Up")
{
	shooter_ = shooter;
	speed_ = speed;
	wait_time_ = wait_time;
	timer_ = new Timer;
	Requires(shooter);
	SetInterruptible(true);
}

// Main functions:
void SpinUp::Initialize()
{
	State_t currState = shooter_->GetState(); // For readability
	if (currState == State_t::OFF || currState == State_t::SPINNINGUP || currState == State_t::SPUNUP)
	{
		TextLog::Log(MessageData(MessageData::INFO), SystemData("Shooter", "SpinUp", "Command")) <<
						"Initializing SpinUp with initial speed: " << speed_ << " and initial wait time: " << wait_time_;
		shooter_->SpinUp(GetSpeed());

		timer_->Reset();
		timer_->Start();
		if (currState != State_t::SPINNINGUP)
			shooter_->SetState(State_t::SPINNINGUP);
	}
	else
	{
		TextLog::Log(MessageData(MessageData::ERR), SystemData("Shooter", "SpinUp", "Command")) <<
				"Failed to initialize SpinUp : Invalid starting state (should be \"OFF\", \"SPINNINGUP\" " <<
				"or \"SPUNUP\")";
		Cancel();
	}
}

bool SpinUp::IsFinished()
{
	return (!shooter_->IsAllowable()) || (timer_->Get() > wait_time_);
}

void SpinUp::End()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Shooter", "SpinUp", "Command")) <<
			"Ending SpinUp";
	shooter_->SetState(State_t::SPUNUP);
}

void SpinUp::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Shooter", "SpinUp", "Command")) <<
			"Interrupting SpinUp";
	shooter_->EmergencyStop();
	shooter_->SetState(State_t::OFF);
}

void SpinUp::SetSpeed(double speed)
{
	speed_ = speed;
}

double SpinUp::GetSpeed() const
{
	if(speed_ < 0.0)
		return shooter_->GetShootPercent();
	else
		return speed_;
}

}// end namespace commands
