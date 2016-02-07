#include "PushBall.h"

using PushBall = commands::PushBall;

// PushBall Command Group:
PushBall::PushBall(Intake *intake) : CommandGroup("PushBall")
{
	intake_ = intake;
	push_ = new Push(intake);
	stop_ = new Stopper(intake);
	timeout_ = 0.0;

	this->AddSequential(push_);
	this->AddSequential(new WaitCommand(timeout_));
	this->AddSequential(stop_);
}
/*
bool PushBall::IsFinished()
{
	return false;
}
/*
void PushBall::Initialize()
{
//	intake_->OutakeBall();
}

bool PushBall::IsFinished()
{
	//wait
//	return !intake_->CheckSwitch();
	return push_->IsFinished();
}

void PushBall::End()
{
//	intake_->Stop();
	// call push end
}

void PushBall::Interrupted()
{
	End();
}
*/
void PushBall::SetTimeout(double timeout)
{
	timeout_ = timeout;
}

double PushBall::GetTimeout() const
{
	return timeout_;
}

// Push Command:
PushBall::Push::Push(Intake *intake)
{
	intake_ = intake;
}

void PushBall::Push::Initialize()
{
	intake_->OutakeBall();
}

bool PushBall::Push::IsFinished()
{
	return !intake_->CheckSwitch();
}
/*
void PushBall::Push::End()
{

}

void PushBall::Push::Interrupted()
{
}*/

// Stopper Command:
PushBall::Stopper::Stopper(Intake *intake)
{
	intake_ = intake;
}
/*
void PushBall::Stopper::Initialize()
{

}*/

bool PushBall::Stopper::IsFinished()
{
	return true;
}

void PushBall::Stopper::End()
{
	intake_->Stop();
}
/*
void PushBall::Stopper::Interrupted()
{

}*/

// Stop Command:
