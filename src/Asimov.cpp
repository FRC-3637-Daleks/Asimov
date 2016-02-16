#include "WPILib.h"
#include "Subsystems/Intake.h"
#include "Subsystems/Shooter.h"
#include "Commands/IntakeBall.h"
#include "Commands/PushBall.h"
#include "Commands/SpinUp.h"
#include "Commands/SpinDown.h"
#include "Commands/Shoot.h"

#include <math.h>
#include <vector>

using Intake = subsystems::Intake;
using Shooter = subsystems::Shooter;
using IntakeBall = commands::IntakeBall;

#include "Subsystems/Drive.h"

#include <memory>

class Asimov: public IterativeRobot
{
private:
	Joystick left_stick_, right_stick_;
	bool tank_drive_;
	subsystems::Drive drive_;

public:
	Asimov(): left_stick_(0), right_stick_(1), tank_drive_(false)
	{
	}

private:
	// Init
	void RobotInit() override
	{
		drive_.Initialize();

		intake_ = new Intake();
		shoot_ = new Shooter();
		xbox_ = new Joystick(2);
		mode = AUTO;
		lock = false;

		intake_->SetMode(Intake::Mode_t::VBUS);

		shoot_->SetAllowedError(900);

		commands.push_back(intake_->MakeIntakeBall());
		triggers.push_back(new JoystickButton(xbox_, A));
		triggers.back()->WhenPressed(commands.back());

		triggers.push_back(new JoystickButton(xbox_, START));
		triggers.back()->CancelWhenPressed(commands.back());

		auto push_ball_command = intake_->MakePushBall();

		triggers.push_back(new JoystickButton(xbox_, B));
		commands.push_back(push_ball_command);
		triggers.back()->WhenPressed(commands.back());

		triggers.push_back(new JoystickButton(xbox_, Y));
		commands.push_back(shoot_->MakeSpinUp());
		triggers.back()->WhenPressed(commands.back());

		triggers.push_back(new JoystickButton(xbox_, X));
		triggers.back()->CancelWhenPressed(commands.back());

		commands.push_back(shoot_->MakeSpinDown());
		triggers.back()->WhenPressed(commands.back());

		triggers.push_back(new JoystickButton(xbox_, R_TRIGGER));
		commands.push_back(new commands::Shoot(intake_, shoot_, 2));
		triggers.back()->WhenPressed(commands.back());

		intake_->SetShootVelocity(1.0);

		intake_->Initialize();
		shoot_->Initialize();

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
		TestDriveInit();
	}

	void TeleopPeriodic() override
	{
		Scheduler::GetInstance()->Run();
		TestDrivePeriodic();
		UpdateDash();
	}

	// Autonomous
	void AutonomousInit() override
	{

	}

	void AutonomousPeriodic() override
	{
		Scheduler::GetInstance()->Run();
	}

	void TestInit()
	{
		TestBoulderInit();
		TestDriveInit();
	}

	void TestPeriodic()
	{
		TestBoulderPeriodic();
		TestDrivePeriodic();
	}

	// Test
	void TestBoulderInit()
	{
	}

	void TestDriveInit()
	{
		drive_.Configure();
		drive_.SetMode(subsystems::Drive::Mode_t::Velocity);
	}

	void TestDrivePeriodic()
	{
		if(left_stick_.GetButton(Joystick::ButtonType::kTriggerButton) ||
				right_stick_.GetButton(Joystick::ButtonType::kTriggerButton))
		{
			drive_.SetMode(subsystems::Drive::Mode_t::VBus);
		}
		else
			drive_.SetMode(subsystems::Drive::Mode_t::Velocity);


		if(left_stick_.GetRawButton(7))
			tank_drive_ = true;
		else if(right_stick_.GetRawButton(7))
			tank_drive_ = false;


		if(tank_drive_)
			drive_.TankDrive(left_stick_.GetY() * fabs(left_stick_.GetY()),
							right_stick_.GetY() * fabs(right_stick_.GetY()));
		else
			drive_.ArcadeDrive(right_stick_.GetY() * fabs(right_stick_.GetY()),
							left_stick_.GetX() * fabs(left_stick_.GetX()));

		UpdateDriveDash();
	}

	void UpdateDriveDash()
	{
		SmartDashboard::PutNumber("Left Setpoint", drive_.GetLeftSetPointRPM());
		SmartDashboard::PutNumber("Left Speed", drive_.GetLeftRPM());

		SmartDashboard::PutNumber("Right Setpoint", drive_.GetRightSetPointRPM());
		SmartDashboard::PutNumber("Right Speed", drive_.GetRightRPM());
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
		SmartDashboard::PutNumber("Shooter Error", shoot_->GetErr());
	}

private:
	Intake *intake_;
	Joystick *xbox_;
	Shooter *shoot_;
	std::vector<Command*> commands;
	std::vector<JoystickButton*> triggers;

	enum Modes {AUTO = 0, MANUAL, PUSH};
	Modes mode;
	enum Button {A = 1, B, X, Y, L_TRIGGER, R_TRIGGER, BACK, START, L_STICK, R_STICK};
	enum Axes {L_XAXIS = 0, L_YAXIS, L_TRIG, R_TRIG, R_XAXIS, R_YAXIS};
	double speed;
	bool lock;
};

START_ROBOT_CLASS(Asimov)
