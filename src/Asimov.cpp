#include "Log/SystemData.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "WPILib/WPISystem.h"
#include "Config/RootSystem.h"
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
#include "Commands/HoldSwiss.h"
#include "Commands/SetSwiss.h"
#include "Commands/SetCamera.h"
#include "Commands/ControlSwissVelocity.h"
#include "Commands/ShootMode.h"
#include "Subsystems/Drive.h"
#include "Subsystems/Camera.h"
#include "Subsystems/Swiss.h"
#include "Subsystems/OI.h"
#include "WPILib/GenericTrigger.h"
#include "Subsystems/Align.h"
#include "Subsystems/Dashboard.h"

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
	Camera camera_;
	Shooter shooter_;
	Swiss swissCheez;
	Align align_;
	Dashboard dashboard_;

private:  // test modes and other things which will become outsourced to other classes
	bool tank_drive_;
	enum Modes {AUTO = 0, MANUAL, PUSH};
	Modes mode;
	double speed;
	bool lock;

private:  // commands and triggers
	CommandGroup auton_command_;
	std::vector<Command*> commands;
	std::vector<Trigger*> triggers;

public:
	Asimov(): RootSystem("/home/lvuser/dman/dalek/"),
		tank_drive_(false), mode(MANUAL), speed(0.0), lock(false)
	{
		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
						"Constructor Started";

		// Subsystems
		AddSubSystem("Drive", drive_);
		AddSubSystem("OI", oi_);
		AddSubSystem("Align", align_);
		AddSubSystem("Camera", camera_);
		AddSubSystem("Swiss", swissCheez);
		AddSubSystem("Dashboard", dashboard_);

		// Port Spaces
		get_context().RegisterPortSpace("CAN", std::make_shared<PortSpace>(0, 63));
		get_context().RegisterPortSpace("USB", std::make_shared<PortSpace>(0, 5));
		get_context().RegisterPortSpace("AnalogIn", std::make_shared<PortSpace>(0, 4));
		get_context().RegisterPortSpace("PWM", std::make_shared<PortSpace>(0, 9));

		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
								"Constructor Complete";
	}

private:
	// Init
	void RobotInit() override
	{
		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit Started";

		shooter_.Initialize();
		intake_.Initialize();

		shooter_.SetMode(Shooter::Mode_t::VELOCITY);
		intake_.SetMode(Intake::Mode_t::VBUS);

		intake_.SetShootVelocity(0.6);
		intake_.SetIntakeSpeed(0.6);
		shooter_.SetAllowedError(1000);

		mode = MANUAL;
		lock = false;

		Register();
		get_context().SaveSchema();


		BindControls();
		BindDashboard();

		TextLog::Log(MessageData(MessageData::INFO, 1), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit Complete";

	}

	void doRegister() override
	{
		RootSystem::doRegister();

		auto& settings = GetSettings();
		settings["Shoot"]("shoot_speed").SetDefault(0.93);

		{
			auto& auton = GetSettings()["autonomous"];

			{
				auto& approach = auton["A_approach"];
				approach("speed").SetDefault(0.5);
				approach("distance").SetDefault(1.8);
				approach("timeout").SetDefault(3.0);
			}

			{
				auto& alignment = auton["B_alignment"];
				alignment("skip").SetDefault(false);
				alignment("timeout").SetDefault(2.0);
				alignment("speed_factor").SetDefault(0.5);
			}

			{
				auto& swiss = auton["C_swiss"];
				swiss("skip").SetDefault(false);
				swiss("timeout").SetDefault(3.0);
			}

			{
				auto& cross = auton["D_cross"];
				cross("speed").SetDefault(.5);
				cross("distance").SetDefault(3.0);
				cross("timeout").SetDefault(5.0);
			}
		}
	}

	bool doConfigure() override
	{
		RootSystem::doConfigure();

		{
			auto& auton = GetSettings()["autonomous"];

			{
				auto& approach = auton["A_approach"];
				double speed = approach("speed").GetValueOrDefault<double>();
				double distance = approach("distance").GetValueOrDefault<double>();
				double timeout = approach("timeout").GetValueOrDefault<double>();

				// Approach the outerworks
				auton_command_.AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);
			}

			{
				auto& alignment = auton["B_alignment"];
				bool skip = alignment("skip").GetValueOrDefault<bool>();
				double timeout = alignment("timeout").GetValueOrDefault<double>();
				double speed_factor = alignment("speed_factor").GetValueOrDefault<double>();

				// Align with outerworks
				if(!skip)
				{
					auton_command_.AddSequential(drive_.MakeTankDrive(GetLocalValue<double>("Align/left_output"),
														GetLocalValue<double>("Align/right_output"),
														speed_factor), timeout);
				}
			}

			{
				auto& swiss = auton["C_swiss"];
				bool skip = swiss("skip").GetValueOrDefault<bool>();
				double timeout = swiss("timeout").GetValueOrDefault<double>();

				// Put swiss down
				if(!skip)
				{
					auton_command_.AddSequential(swissCheez.MakeSetSwiss(Swiss::state_t::port_down), timeout);
				}
			}

			{
				auto& cross = auton["D_cross"];
				double speed = cross("speed").GetValueOrDefault<double>();
				double distance = cross("distance").GetValueOrDefault<double>();
				double timeout = cross("timeout").GetValueOrDefault<double>();

				// Drive over defense
				auton_command_.AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);
			}
		}

		return true;
	}

	void BindControls()
	{
		drive_.SetDefaultCommand(drive_.MakeArcadeDrive(oi_.GetArcadeForwardAxis(), oi_.GetArcadeTurnAxis(), 1.0, 1.0));
		swissCheez.SetDefaultCommand(swissCheez.MakeHoldSwiss());
		camera_.SetDefaultCommand(camera_.MakeSetCamera(Camera::CamState_t::WHEEL));

		CommandGroup *intake_group = new CommandGroup;
		intake_group->AddParallel(camera_.MakeSetCamera(Camera::CamState_t::BALL));
		intake_group->AddParallel(intake_.MakeIntakeBall());
		commands.push_back(intake_group);
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/intake")));
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/START")));
		triggers.back()->CancelWhenActive(commands.back());

		auto push_ball_command = intake_.MakePushBall();

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/push")));
		commands.push_back(push_ball_command);
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/spin_up")));

		CommandGroup *spinup_group = new CommandGroup;
		spinup_group->AddParallel(shooter_.MakeSpinUp(GetSettings()["Shoot"]("shoot_speed").GetValueOrDefault<double>()));
		spinup_group->AddParallel(oi_.MakeShootMode(&camera_, .3, .3));

		commands.push_back(spinup_group);
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/spin_down")));
		triggers.back()->CancelWhenActive(commands.back());

		commands.push_back(shooter_.MakeSpinDown());
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/shoot")));
		commands.push_back(new commands::Shoot(&intake_, &shooter_, 2.0));
		triggers.back()->WhenActive(commands.back());

		commands.push_back(oi_.MakeForwardBoost(2.0));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/forward_boost")));
		triggers.back()->WhileActive(commands.back());

		commands.push_back(oi_.MakeTurnBoost(2.0));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/turn_boost")));
		triggers.back()->WhileActive(commands.back());

		commands.push_back(new commands::FlipFront(&oi_, &camera_));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/reverse_mode")));
		triggers.back()->WhenActive(commands.back());
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/front_mode")));
		triggers.back()->CancelWhenActive(commands.back());

		// auto align
		commands.push_back(drive_.MakeArcadeDrive(GetLocalValue<double>("Align/forward_output"),
												oi_.GetNullAxis(), .2, 1.0));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/forward_sensor_align")));
		triggers.back()->WhileActive(commands.back());

		commands.push_back(drive_.MakeArcadeDrive(oi_.GetNullAxis(), GetLocalValue<double>("Align/turn_output"),
													1.0, .3));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/turn_sensor_align")));
		triggers.back()->WhileActive(commands.back());

		// swiss
		for(int state = Swiss::retract; state < Swiss::n_states; state++)
		{
			commands.push_back(swissCheez.MakeSetSwiss(static_cast<Swiss::state_t>(state)));
			triggers.push_back(new GenericTrigger(oi_.GetSwissStateButton(static_cast<Swiss::state_t>(state))));
			triggers.back()->WhenActive(commands.back());
		}

		commands.push_back(swissCheez.MakeControlSwissVelocity(oi_.GetSwissAxis()));
		triggers.push_back(new GenericTrigger(oi_.GetControlSwissButton()));
		triggers.back()->WhileActive(commands.back());
	}

	void BindDashboard()
	{
		std::string double_values[] =
		{"Robot/Align/left_error", "Robot/Align/right_error", "Robot/Align/forward_error", "Robot/Align/rotation_error",
		"Robot/Swiss/talon/output_current", "Robot/Swiss/talon/output_speed", "Robot/Swiss/talon/output_voltage"};
		for(auto double_value : double_values)
			dashboard_.AddDashValue<double>(double_value);

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
		auton_command_.Start();
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
<<<<<<< HEAD
=======
			intake_.SetMode(Intake::Mode_t::VELOCITY);
>>>>>>> dev-DQsevilla
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
