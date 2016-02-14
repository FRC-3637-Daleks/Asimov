#include "WPILib.h"

#include "Subsystems/Intake.h"
#include "Subsystems/Shooter.h"

class Asimov: public IterativeRobot
{
private:
	subsystems::Intake intake_;
	Joystick gamepad_;

public:
	Asimov(): gamepad_(2) {}

private:
	// Init
	void RobotInit() override
	{
		intake_.Initialize();
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
	void TestInit() override
	{
		intake_.SetMode(subsystems::Intake::Mode_t::VBUS);
	}

	void TestPeriodic() override
	{
		if(gamepad_.GetRawButton(1))
		{
			intake_.TakeBall(true);
		}
		else if(gamepad_.GetRawButton(2))
		{
			intake_.TakeBall(false);
		}
		else if(gamepad_.GetRawButton(3))
		{
			intake_.OutakeBall();
		}
		else
		{
			intake_.SetSpeed(gamepad_.GetRawAxis(0));
		}

	}
};

START_ROBOT_CLASS(Asimov)

