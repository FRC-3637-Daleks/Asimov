#include "PushBall.h"

/**
 * Commands namespace with implementation
 * of PushBall command group, and nested
 * Push and Stopper command classes.
 */
namespace commands
{

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
	std::cout << "Intake : PushBall : Started with timeout = " << std::endl;
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
	std::cout << "Intake : PushBall : Push : Started with timeout " << timeout_ << std::endl;
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
	std::cout << "Intake : PushBall : Push : Ended" << std::endl;
}

// Stopper constructor:
PushBall::Stopper::Stopper(Intake *intake)
{
	std::cout << "Intake : PushBall : Stopper : Started" << std::endl;
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
	std::cout << "Intake : PushBall : Stopper : Ended" << std::endl;
	intake_->Stop();
	intake_->SetState(State_t::OFF);
}

} // end namespace commands
