#include <Commands/Shoot.h>

/**
 * Commands namespace with implementation
 * of Shoot command group.
 */
namespace commands
{

// Constructor:
Shoot::Shoot(Intake* intake, Shooter* shooter, double wait_time, double shoot_time, double timeout) : CommandGroup("Shoot")
{
	intake_ = intake;
	shooter_ = shooter;

	shoot_time_ = shoot_time;
	timeout_ = timeout;
	wait_time_ = wait_time;

	shoot_intake_ = new ShootIntake(intake_, shooter_, shoot_time_, timeout_);
	wait_ = new WaitCommand(wait_time_);
	spin_down_ = new SpinDown(shooter_);

	AddSequential(shoot_intake_);
	AddSequential(wait_);
	AddSequential(spin_down_);

	Requires(intake);
	SetInterruptible(false);
}

// Main functions:
void Shoot::Initialize()
{
	std::cout << "Intake and Shooter : Shoot : Started with wait time = " << wait_time_ << ", shoot time = ";
	std::cout << shoot_time_ << ", and timeout = " << timeout_ << std::endl;
	if (shooter_->GetState() != State_t::SPUNUP)
		Cancel();
}

void Shoot::SetShootTime(double shoot_time)
{
	shoot_time_ = shoot_time;
	shoot_intake_->SetShootTime(shoot_time_);
}

double Shoot::GetShootTime() const
{
	return shoot_time_;
}

void Shoot::SetTimeOut(double timeout)
{
	timeout_ = timeout;
	shoot_intake_->SetOverallTimeout(timeout_);
}

double Shoot::GetTimeOut() const
{
	return timeout_;
}

void Shoot::SetWaitTime(double wait_time)
{
	wait_time_ = wait_time;
}

double Shoot::GetWaitTime() const
{
	return wait_time_;
}

} // end namespace commands
