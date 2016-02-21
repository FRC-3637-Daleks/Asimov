#include "Log/SystemData.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "WPILib/WPISystem.h"
#include "Config/PortSpace.h"

#include "WPILib.h"

#include "Subsystems/Drive.h"

#include <memory>

using namespace dman;

class Asimov: public IterativeRobot, public RootSystem
{
private:
	Joystick gamepad_, left_stick_, right_stick_;
	bool tank_drive_;
	subsystems::Drive drive_;

public:
	Asimov(): RootSystem("/home/lvuser/dalek/"), gamepad_(2), left_stick_(0), right_stick_(1), tank_drive_(false)
	{
		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
						"Constructor Started";

		AddSubSystem("Drive", drive_);
		get_context().RegisterPortSpace("CAN", std::make_shared<PortSpace>(0, 63));

		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
								"Constructor Complete";
	}

private:
	// Init
	void RobotInit() override
	{
		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit Started";

		Register();
		get_context().SaveSchema();

		TextLog::Log(MessageData(MessageData::INFO, 1), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit Complete";
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
		Configure();
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

