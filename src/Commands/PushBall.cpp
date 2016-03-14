#include "PushBall.h"

/**
 * Commands namespace with implementation
 * of PushBall command group, and nested
 * Push and Stopper command classes.
 */
namespace commands
{

// PushBall constructor:
PushBall::PushBall(Intake *intake, double time) : CommandGroup("PushBall")
{
	intake_ = intake;
	timeout_ = time;
	push_ = new Push(intake);
	stop_ = new Stopper(intake);
	wait_ = new WaitCommand(timeout_);

	SetInterruptible(false);

	this->AddSequential(push_);
	this->AddSequential(wait_);
	this->AddSequential(stop_);
}

// PushBall main functions:
void PushBall::Initialize()
{
	std::cout << "Intake : PushBall : Started with timeout = " << timeout_ << std::endl;
	if (1)
	{

	}
}
double PushBall::GetAddedTime() const
{
	return timeout_;
}

// Push constructor:
PushBall::Push::Push(Intake *intake)
{
	intake_ = intake;
	canceled = false;
	Requires(intake);
	SetInterruptible(false);
}

// Push main functions
void PushBall::Push::Initialize()
{
	std::cout << "Intake : PushBall : Push : Started" << std::endl;
	intake_->SetState(State_t::PUSHING);
	intake_->OutakeBall();
}

bool PushBall::Push::IsFinished()
{
	return canceled || !intake_->CheckSwitch();
}

void PushBall::Push::End()
{
	canceled = false;
	std::cout << "Intake : PushBall : Push : Ended" << std::endl;
}

void PushBall::Push::SetCanceled()
{
	canceled = true;
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
