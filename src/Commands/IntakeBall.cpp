#include "IntakeBall.h"

using IntakeBall = commands::IntakeBall;

// Constructor:
IntakeBall::IntakeBall(Intake * intake) : Command("IntakeBall")
{
	intake_ = intake;
}

void IntakeBall::Initialize()
{
	intake_->TakeBall(true);
}

void IntakeBall::Execute()
{

}

bool IntakeBall::IsFinished()
{
	return intake_->CheckSwitch();
}

void IntakeBall::End()
{
	intake_->Stop();
}

void IntakeBall::Interrupted()
{
	End();
}
