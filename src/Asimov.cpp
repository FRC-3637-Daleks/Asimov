#include "Log/SystemData.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"

#include "WPILib.h"

#include "Subsystems/Drive.h"

#include <memory>

class Asimov: public IterativeRobot
{
private:
	Joystick gamepad_, left_stick_, right_stick_;
	bool tank_drive_;
	subsystems::Drive drive_;

public:
	Asimov(): gamepad_(2), left_stick_(0), right_stick_(1), tank_drive_(false)
	{
	}

private:
	// Init
	void RobotInit() override
	{
		using namespace dman;

		drive_.Initialize();

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
		TestDriveInit();
	}

	void TestPeriodic() override
	{
		TestDrivePeriodic();
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
};

START_ROBOT_CLASS(Asimov)

