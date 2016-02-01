#include "WPILib.h"

class Asimov: public IterativeRobot
{
public:
	Asimov() {}

private:
	// Init
	void RobotInit() override
	{

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

	}

	void TestPeriodic() override
	{

	}
};

START_ROBOT_CLASS(Asimov)

