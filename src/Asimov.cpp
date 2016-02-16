#include "Log/SystemData.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"

#include "WPILib.h"

class Asimov: public IterativeRobot
{
public:
	Asimov() {}

private:
	// Init
	void RobotInit() override
	{
		using namespace dman;
		TextLog::Log(MessageData(MessageData::INFO, 0), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit complete";
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

