#include "Log/SystemData.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "WPILib/WPISystem.h"
#include "Config/PortSpace.h"
#include "Utility/ValueStore.h"
#include "Utility/Valuable.h"

#include "WPILib.h"

#include "Subsystems/Intake.h"
#include "Subsystems/Shooter.h"
#include "Commands/IntakeBall.h"
#include "Commands/PushBall.h"
#include "Commands/SpinUp.h"
#include "Commands/SpinDown.h"
#include "Commands/Shoot.h"
#include "Commands/ForwardBoost.h"
#include "Commands/TurnBoost.h"
#include "Commands/FlipFront.h"
#include "Subsystems/Drive.h"
#include "Subsystems/CameraMount.h"
#include "Subsystems/Swiss.h"
#include "Subsystems/OI.h"
#include "WPILib/GenericTrigger.h"

#include <math.h>
#include <vector>
#include <memory>

using namespace dman;
using namespace subsystems;

class Asimov: public IterativeRobot, public RootSystem
{
private:  // subsystems
	OI oi_;
	Drive drive_;
	Intake intake_;
	CameraMount mount_;
	Shooter shooter_;
	Swiss swissCheez;

private:  // test modes and other things which will become outsourced to other classes
	bool tank_drive_;
	enum Modes {AUTO = 0, MANUAL, PUSH};
	Modes mode;
	double speed;
	bool lock;

private:  // commands and triggers
	Command * auton_command_;
	std::vector<Command*> commands;
	std::vector<Trigger*> triggers;

public:
	Asimov(): RootSystem("/home/lvuser/dalek/"),
		tank_drive_(false), mode(AUTO), speed(0.0), lock(false)
	{
		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
						"Constructor Started";

		AddSubSystem("Drive", drive_);
		AddSubSystem("OI", oi_);
		get_context().RegisterPortSpace("CAN", std::make_shared<PortSpace>(0, 63));

		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
								"Constructor Complete";
		std::cout << "I scream" << std::endl;
	}

private:
	// Init
	void RobotInit() override
	{
		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit Started";

		mount_.doConfigure();
		shooter_.Initialize();
		intake_.Initialize();

		shooter_.SetMode(Shooter::Mode_t::VELOCITY);
		intake_.SetMode(Intake::Mode_t::VBUS);

		intake_.SetShootVelocity(0.6);
		intake_.SetIntakeSpeed(0.6);
		shooter_.SetAllowedError(1000);

		mode = AUTO;
		lock = false;

		drive_.SetDefaultCommand(drive_.MakeArcadeDrive(oi_.GetArcadeForwardAxis(), oi_.GetArcadeTurnAxis(), 1.0, 1.0));

		BindControls();

		// auton_command_ = drive_.MakeDriveStraight(0.5, Drive::Meters_t(6));  // Drive at 50% speed for 6 seconds

		Register();
		get_context().SaveSchema();

		TextLog::Log(MessageData(MessageData::INFO, 1), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit Complete";

	}

	void BindControls()
	{
		commands.push_back(intake_.MakeIntakeBall());
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/A")));
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/START")));
		triggers.back()->CancelWhenActive(commands.back());

		auto push_ball_command = intake_.MakePushBall();

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/B")));
		commands.push_back(push_ball_command);
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/Y")));
		commands.push_back(shooter_.MakeSpinUp(0.95));
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/X")));
		triggers.back()->CancelWhenActive(commands.back());

		commands.push_back(shooter_.MakeSpinDown());
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/R_TRIGGER")));
		commands.push_back(new commands::Shoot(&intake_, &shooter_, 2.0));
		triggers.back()->WhenActive(commands.back());

		commands.push_back(oi_.MakeForwardBoost(1.0));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/driver_right/buttons/1")));
		triggers.back()->WhileActive(commands.back());

		commands.push_back(oi_.MakeTurnBoost(1.0));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/driver_left/buttons/1")));
		triggers.back()->WhileActive(commands.back());

		commands.push_back(new commands::FlipFront(&oi_, &mount_));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/driver_right/buttons/10")));
		triggers.back()->WhenActive(commands.back());
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/driver_right/buttons/11")));
		triggers.back()->CancelWhenActive(commands.back());

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
		Configure();
		TestDriveInit();
	}

	void TeleopPeriodic() override
	{
		Scheduler::GetInstance()->Run();
		UpdateDash();
		//TestSwiss();
	}

	// Autonomous
	void AutonomousInit() override
	{
		Configure();
		auton_command_->Start();
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
		TestSwiss();
	}

	// Test
	void TestBoulderInit()
	{
	}

	void TestDriveInit()
	{
		drive_.SetMode(Drive::Mode_t::Velocity);
	}

	void TestDrivePeriodic()
	{
		if(oi_.get_left().GetButton(Joystick::ButtonType::kTriggerButton) ||
				oi_.get_right().GetButton(Joystick::ButtonType::kTriggerButton))
		{
			drive_.SetMode(subsystems::Drive::Mode_t::VBus);
		}
		else
			drive_.SetMode(subsystems::Drive::Mode_t::Velocity);


		if(oi_.get_left().GetRawButton(7))
			tank_drive_ = true;
		else if(oi_.get_right().GetRawButton(7))
			tank_drive_ = false;


		if(tank_drive_)
			drive_.TankDrive(oi_.get_left().GetY() * fabs(oi_.get_left().GetY()),
							oi_.get_right().GetY() * fabs(oi_.get_right().GetY()));
		else
			drive_.ArcadeDrive(oi_.get_right().GetY() * fabs(oi_.get_right().GetY()),
							oi_.get_left().GetX() * fabs(oi_.get_left().GetX()));


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
		if (oi_.get_xbox().GetRawButton(XBoxWrapper::A))
		{
			mode = AUTO;
		}

		else if (oi_.get_xbox().GetRawButton(XBoxWrapper::B))
		{
			mode = MANUAL;
		}

		else if (oi_.get_xbox().GetRawButton(XBoxWrapper::X))
		{
			mode = PUSH;
		}

		if (oi_.get_xbox().GetRawButton(XBoxWrapper::L_TRIGGER))
		{
			lock = true;
		}

		else if(oi_.get_xbox().GetRawButton(XBoxWrapper::BACK))
		{
			lock = false;
		}

		if(oi_.get_xbox().GetRawButton(XBoxWrapper::START))
		{
			shooter_.SetMode(Shooter::Mode_t::VBUS);
		}
		else
		{
			shooter_.SetMode(Shooter::Mode_t::VELOCITY);
		}

		if(!lock)
		{
			speed = oi_.get_xbox().GetRawAxis(XBoxWrapper::L_TRIG);
		}

		shooter_.SpinUp(speed);

		switch (mode)
		{
		case AUTO:
			intake_.TakeBall(true);
			break;
		case MANUAL:
			if (fabs(oi_.get_xbox().GetRawAxis(XBoxWrapper::R_YAXIS)) < 0.05)
				intake_.Stop();
			else
				intake_.SetSpeed(oi_.get_xbox().GetRawAxis(XBoxWrapper::R_YAXIS)); //needs to be right stick
			break;
		case PUSH:
			intake_.OutakeBall();
			break;
		}

		UpdateDash();
	}

	void UpdateDash()
	{
		SmartDashboard::PutNumber("Shooter Error", shooter_.GetErr());
	}


	void TestSwiss()
	{
		int pov = oi_.get_xbox().GetPOV();
		if (pov >= 0) {
			if(pov < 30){
				swissCheez.SetState(Swiss::state_t::horizontal);
			}
			else if(pov < 120){
				swissCheez.SetState(Swiss::state_t::cheval_down);
			}
			else if(pov < 210){
				swissCheez.SetState(Swiss::state_t::port_down);
			}
			else if(pov < 300){
				swissCheez.SetState(Swiss::state_t::retract);
			}

		} else if(fabs(oi_.get_xbox().GetRawAxis(XBoxWrapper::L_YAXIS)) > 0.3) {
			swissCheez.SetMode(Swiss::mode_t::vbus);
			swissCheez.SetVelocity(-oi_.get_xbox().GetRawAxis(XBoxWrapper::L_YAXIS), false);
		}
		else if (swissCheez.GetMode() != Swiss::mode_t::pos)
		{
			swissCheez.SetVelocity(0.0, false);
		}
	}
};

START_ROBOT_CLASS(Asimov)
