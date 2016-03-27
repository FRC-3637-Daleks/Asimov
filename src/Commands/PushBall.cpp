#include "PushBall.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

/**
 * Commands namespace with implementation
 * of PushBall command group, and nested
 * Push and Stopper command classes.
 */
namespace commands
{

using namespace dman;

// PushBall constructor:
PushBall::PushBall(Intake *intake) : CommandGroup("PushBall")
{
	intake_ = intake;
	push_ = new Push(intake);
	stop_ = new Stopper(intake);

	SetInterruptible(false);

	this->AddSequential(push_);
	this->AddSequential(stop_);
}

// PushBall main functions:
void PushBall::Initialize()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "PushBall", "CommandGroup")) <<
				"Initializing PushBall";
//	std::cout << "Intake : PushBall : Started with timeout = " << std::endl;
	if (1)
	{

	}
}

// Push constructor:
PushBall::Push::Push(Intake *intake, double timeout)
{
	intake_ = intake;
	target_position_ = 30.0;
	timeout_ = timeout;
	Requires(intake);
	SetInterruptible(false);
}

// Push main functions
void PushBall::Push::Initialize()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "Push", "Command")) <<
				"Initializing Push (of PushBall) with initial time out: " << timeout_;
	intake_->SetState(State_t::PUSHING);
	intake_->OutakeBall();
}

bool PushBall::Push::IsFinished()
{
	SetTimeout(timeout_);
	return (!intake_->CheckSwitch() && intake_->GetCurrentPosition() >= target_position_) || IsTimedOut();
}

void PushBall::Push::End()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "Push", "Command")) <<
				"Ending Push (of PushBall)";
}

// Stopper constructor:
PushBall::Stopper::Stopper(Intake *intake)
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "Stopper", "Command")) <<
				"Initializing Stopper (of PushBall)";
	intake_ = intake;
	Requires(intake);
	SetInterruptible(false);
}

// Stopper main functions:
bool PushBall::Stopper::IsFinished()
{
	return true;
}

void PushBall::Stopper::End()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Intake", "Stopper", "Command")) <<
				"Ending Stopper (of PushBall)";
	intake_->Stop();
	intake_->SetState(State_t::OFF);
}

} // end namespace commands
