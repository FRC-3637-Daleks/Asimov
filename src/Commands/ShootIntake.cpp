#include "Commands/ShootIntake.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

/**
 * Commands namespace with implementation
 * of ShootIntake command.
 */
namespace commands
{

using namespace dman;

// Constructor:
ShootIntake::ShootIntake(Intake *intake, Shooter *shooter, double shoot_time, double timeout) : Command("ShootIntake")
{
	intake_ = intake;
	shooter_ = shooter;

	timeout_ = timeout;
	shoot_time_ = shoot_time;
	check_switch_ = true;

	timer_ = new Timer;

	Requires(intake);
	SetTimeout(timeout_);
	SetInterruptible(false);
}

// Main functions:
void ShootIntake::Initialize()
{
	if (shooter_->GetState() == Shooter::State_t::SPUNUP && intake_->GetState() == Intake::State_t::HOLDING)
	{
		TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake and Shooter", "ShootIntake", "Command")) <<
				"Initializing ShootIntake with initial shoot time: " << shoot_time_ << " and initial time out: "
				<< timeout_;

		timer_->Reset();
		shooter_->SetState(Shooter::State_t::SHOOTING);
		intake_->SetState(Intake::State_t::SHOOTING);
		intake_->ShootBall();
		check_switch_ = false;
	}
	else
	{
		TextLog::Log(MessageData(MessageData::ERR), SystemData("Intake and Shooter", "ShootIntake", "Command")) <<
				"Failed to initialize ShootIntake : Invalid starting state (should be \"SPUNUP\"" <<
				" and \"HOLDING\")";
		Cancel();
	}
}

void ShootIntake::Execute()
{
	if (check_switch_ && !intake_->CheckSwitch())
	{
		timer_->Start();
		check_switch_ = false;
	}
}

void ShootIntake::End()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake and Shooter", "ShootIntake", "Command")) <<
				"Ending ShootIntake";

	if(shooter_->GetState() == Shooter::State_t::SHOOTING)
		shooter_->SetState(Shooter::State_t::SPUNUP);
	intake_->SetState(Intake::State_t::OFF);
	intake_->Stop();
}

bool ShootIntake::IsFinished()
{
	return IsTimedOut() || timer_->Get() >= shoot_time_;
}

void ShootIntake::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake and Shooter", "ShootIntake", "Command")) <<
				"Interrupting ShootIntake";
	if(intake_->GetState() == Intake::State_t::SHOOTING)  // Prevents shooter from ending in wrong state after cancel
		End();
}

void ShootIntake::SetShootTime(double shoot_time)
{
	shoot_time_ = shoot_time;
}

double ShootIntake::GetShootTime() const
{
	return shoot_time_;
}

void ShootIntake::SetOverallTimeout(double time_out)
{
	timeout_ = time_out;
	SetTimeout(timeout_);
}

double ShootIntake::GetOverallTimeout() const
{
	return timeout_;
}

} // end namespace commands
