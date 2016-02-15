#include "WPILib.h"
#include "Subsystems/Intake.h"
#include "Subsystems/Shooter.h"
#include <math.h>

using Intake = subsystems::Intake;
using Shooter = subsystems::Shooter;
using IntakeBall = commands::IntakeBall;

class Asimov: public IterativeRobot
{
public:
	Asimov() {}

private:
	// Init
	void RobotInit() override
	{
		intake_ = new Intake();
		shoot_ = new Shooter();
		xbox_ = new Joystick(1);
		mode = AUTO;
		lock = false;

		intake_->SetMode(Intake::Mode_t::VBUS);

//		take_ = intake_->MakeIntakeBall();
	}

	// Disabled
	void DisabledInit() override
	{
	}

	void DisabledPeriodic() override
	{
		Scheduler::GetInstance()->Run();
	}


	// Teleop
	void TeleopInit() override
	{
	}

	void TeleopPeriodic() override
	{
		Scheduler::GetInstance()->Run();
	}

	// Autonomous
	void AutonomousInit() override
	{

	}

	void AutonomousPeriodic() override
	{
		Scheduler::GetInstance()->Run();
	}

	// Test
	void TestBoulderInit()
	{
		intake_->Initialize();
		shoot_->Initialize();
	}

	void TestBoulderPeriodic()
	{
		if (xbox_->GetRawButton(A))
		{
			mode = AUTO;
		}

		else if (xbox_->GetRawButton(B))
		{
			mode = MANUAL;
		}

		else if (xbox_->GetRawButton(X))
		{
			mode = PUSH;
		}

		if (xbox_->GetRawButton(L_TRIGGER))
		{
			lock = true;
		}

		else if(xbox_->GetRawButton(BACK))
		{
			lock = false;
		}

		if(xbox_->GetRawButton(START))
		{
			shoot_->SetMode(Shooter::Mode_t::VBUS);
		}
		else
		{
			shoot_->SetMode(Shooter::Mode_t::VELOCITY);
		}

		if(!lock)
		{
			speed = xbox_->GetRawAxis(L_TRIG);
		}

		shoot_->SpinUp(speed);

		switch (mode)
		{
		case AUTO:
			intake_->TakeBall(true);
			break;
		case MANUAL:
			if (fabs(xbox_->GetRawAxis(R_YAXIS)) < 0.05)
				intake_->Stop();
			else
				intake_->SetSpeed(xbox_->GetRawAxis(R_YAXIS)); //needs to be right stick
			break;
		case PUSH:
			intake_->OutakeBall();
			break;
		}

		UpdateDash();
	}

	void UpdateDash()
	{
		SmartDashboard::PutNumber("Shooter Speed", shoot_->GetSpeed());
	}

private:
	Intake *intake_;
	Joystick *xbox_;
	Shooter *shoot_;

	enum Modes {AUTO = 0, MANUAL, PUSH};
	Modes mode;
	enum Button {A = 1, B, X, Y, L_TRIGGER, R_TRIGGER, BACK, START, L_STICK, R_STICK};
	enum Axes {L_XAXIS = 0, L_YAXIS, L_TRIG, R_TRIG, R_XAXIS, R_YAXIS};
	double speed;
	bool lock;
};

START_ROBOT_CLASS(Asimov)
