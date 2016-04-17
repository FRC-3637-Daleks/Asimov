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
#include "Commands/ControlIntake.h"
#include "Commands/ShootMode.h"
#include "Subsystems/Drive.h"
#include "Subsystems/Camera.h"
#include "Subsystems/Swiss.h"
#include "Subsystems/OI.h"
#include "Subsystems/Grip.h"
#include "WPILib/GenericTrigger.h"
#include "Subsystems/Align.h"
#include "Subsystems/Dashboard.h"
#include "Config/EnumSetting.h"

#include <math.h>
#include <vector>
#include <memory>

using namespace dman;
using namespace subsystems;

class Asimov: public IterativeRobot, public RootSystem
{
public:
	enum Defenses {LOWBAR=0, PORTCULLIS, CHEVAL, MOAT, RAMPARTS, SALLY, DRAWBRIDGE, TERRAIN, WALL, N};
	enum AutoMode {NO_OP=0, IN, IN_OUT, DEAD_HIGH_GOAL, AUTO_HIGH_GOAL, N_MODES};

private:  // subsystems
	OI oi_;
	Drive drive_;
	Intake intake_;
	Camera camera_;
	Shooter shooter_;
	Swiss swissCheez;
	Align align_;
	Dashboard dashboard_;
	GRIP grip_;

private:  // test modes and other things which will become outsourced to other classes
	bool tank_drive_;
	enum Modes {AUTO = 0, MANUAL, PUSH};
	Modes mode;
	double speed;
	bool lock;

private:  // commands and triggers
	EnumWrapper<Defenses> auton_defense_;
	SettingWrapper<int> auton_position_;
	EnumWrapper<AutoMode, N_MODES> auton_mode_;
	std::unique_ptr<CommandGroup> auton_command_;

	std::vector<Command*> commands;
	std::vector<Trigger*> triggers;

public:
	Asimov(): RootSystem("/home/lvuser/dalek/"),
		tank_drive_(false), mode(MANUAL), speed(0.0), lock(false)
	{
		std::cout << "I'm alive" << std::endl;
		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
						"Constructor Started";

		// Subsystems
		AddSubSystem("Drive", drive_);
		AddSubSystem("OI", oi_);
		AddSubSystem("Align", align_);
		AddSubSystem("CameraMount", camera_);
		AddSubSystem("Swiss", swissCheez);
		AddSubSystem("Intake", intake_);
		AddSubSystem("Shooter", shooter_);
		AddSubSystem("Dashboard", dashboard_);
		AddSubSystem("GRIP", grip_);

		// Port Spaces
		get_context().RegisterPortSpace("CAN", std::make_shared<PortSpace>(0, 63));
		get_context().RegisterPortSpace("USB", std::make_shared<PortSpace>(0, 5));
		get_context().RegisterPortSpace("AnalogIn", std::make_shared<PortSpace>(0, 4));
		get_context().RegisterPortSpace("PWM", std::make_shared<PortSpace>(0, 9));
		get_context().RegisterPortSpace("DIO", std::make_shared<PortSpace>(0, 9));

		// Defense names
		EnumWrapper<Defenses>::SetString(Defenses::LOWBAR, "Low Bar");
		EnumWrapper<Defenses>::SetString(Defenses::PORTCULLIS, "Portcullis");
		EnumWrapper<Defenses>::SetString(Defenses::CHEVAL, "Cheval de Frise");
		EnumWrapper<Defenses>::SetString(Defenses::MOAT, "Moat");
		EnumWrapper<Defenses>::SetString(Defenses::RAMPARTS, "Ramparts");
		EnumWrapper<Defenses>::SetString(Defenses::SALLY, "Sally Port");
		EnumWrapper<Defenses>::SetString(Defenses::DRAWBRIDGE, "Drawbridge");
		EnumWrapper<Defenses>::SetString(Defenses::TERRAIN, "Rough Terrain");
		EnumWrapper<Defenses>::SetString(Defenses::WALL, "Rock Wall");

		// Autonomous Mode Names
		EnumWrapper<AutoMode, N_MODES>::SetString(AutoMode::NO_OP, "no-op");
		EnumWrapper<AutoMode, N_MODES>::SetString(AutoMode::IN, "Cross");
		EnumWrapper<AutoMode, N_MODES>::SetString(AutoMode::IN_OUT, "Cross, Drop Ball, Come Back");
		EnumWrapper<AutoMode, N_MODES>::SetString(AutoMode::DEAD_HIGH_GOAL, "Dead Reckoned High Goal Shot");
		EnumWrapper<AutoMode, N_MODES>::SetString(AutoMode::AUTO_HIGH_GOAL, "Vision Tracked High Goal Shot");

		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
								"Constructor Complete";
	}

private:
	// Init
	void RobotInit() override
	{
		TextLog::Log(MessageData(MessageData::INFO, 2), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit Started";

		//shooter_.SetMode(Shooter::Mode_t::VELOCITY);
		intake_.SetMode(Intake::Mode_t::VELOCITY);

		mode = MANUAL;
		lock = false;

		Register();
		get_context().SaveSchema();
		get_context().AssembleConfig();

		BindControls();
		BindDashboard();

		TextLog::Log(MessageData(MessageData::INFO, 1), SystemData("Asimov", "RobotInit", "Robot")) <<
				"RobotInit Complete";
	}

	void doRegister() override
	{
		RootSystem::doRegister();

		auto& settings = GetSettings();

		{
			auto& auton = settings["autonomous"];

			auton_defense_ = auton.WrapSetting<EnumWrapper<Defenses>>("defense");
			auton_defense_.SetEnumDefault(LOWBAR);

			auton_position_ = auton.WrapSetting<SettingWrapper<int>>("position");
			auton_position_.SetDefault(1);
			{
				auto& position_schema = auton_position_.GetSetting().base_schema_;
				position_schema["minimum"] = 1;
				position_schema["maximum"] = 5;
			}

			auton_mode_ = auton.WrapSetting<EnumWrapper<AutoMode, N_MODES>>("mode");
			auton_mode_.SetEnumDefault(IN_OUT);

			{
				auto& approach = auton["A_approach"];
				approach("speed").SetDefault(0.5);
				approach("distance").SetDefault(1.8);
				approach("timeout").SetDefault(3.0);
			}

			{
				auto& alignment = auton["B_alignment"];
				alignment("skip").SetDefault(true);
				alignment("timeout").SetDefault(2.0);
				alignment("speed_factor").SetDefault(0.5);
			}

			{
				auto& turn_around = auton["TurnAround"];
				turn_around("skip").SetDefault(false);
				turn_around("timeout").SetDefault(3.0);
				turn_around("speed").SetDefault(.7);
				turn_around("left_revs").SetDefault(1.3);
			}

			{
				auto& swiss = auton["Swiss"];
				swiss.SetDescription("This will only apply to auton routines which require use of the swiss");
				swiss("skip").SetDefault(false);
				swiss("timeout").SetDefault(3.0);
			}

			{
				auto& cross = auton["Cross"];
				cross.SetDescription("This should take the robot to be about a foot behind the batter");
				cross("speed").SetDefault(.5);
				cross("distance").SetDefault(3.0);
				cross("timeout").SetDefault(5.0);
			}

			{
				auto& ret = auton["Return"];
				ret("speed").SetDefault(.5);
				ret("distance").SetDefault(5.0);
				ret("short_distance").SetDefault(4.0);
				ret("timeout").SetDefault(5.0);
				ret("leave_ball").SetDefault(true);
				ret.SetDescription("Distance traveled back by robot after spitting out boulder");
			}

			{
				auto& trickshot = auton["Trickshot"];
				trickshot("speed").SetDefault(.25);
				trickshot("distance").SetDefault(.5);
				trickshot("timeout").SetDefault(2.0);
				trickshot.SetDescription("Balance on the cheval in auto B)");
			}

			{
				auto& approaches = auton["CourtyardApproaches"];
				approaches.SetDescription("This is additional distance to be traveled after the initial cross per defense position");
				approaches("speed").SetDefault(.5);
				approaches("timeout").SetDefault(2.0);
				for(int i = 1; i <= 5; i++)
				{
					approaches["from_" + std::to_string(i)]("distance").SetDefault(0.0);
				}
			}

			{
				auto& turns = auton["Turns"];
				turns.SetDescription("Each represents the number of revolutions the left wheel should travel to align the robot with the goal it goes for");
				turns("speed").SetDefault(.5);
				turns("timeout").SetDefault(1.0);
				for(int i = 1; i <= 5; i++)
				{
					turns["from_" + std::to_string(i)]("left_revs").SetDefault(0.0);
				}
			}

			{
				auto& last_approach = auton["last_approach"];
				last_approach.SetDescription("Distances needed to get to the goal");
				last_approach("speed").SetDefault(.5);
				last_approach("timeout").SetDefault(2.0);
				for(int i = 1; i <= 5; i++)
				{
					last_approach["from_" + std::to_string(i)]("distance").SetDefault(.5);
				}
			}
		}
	}

	bool doConfigure() override
	{
		RootSystem::doConfigure();

		{
			/// Note: This has really shitty memory leaks because I have to dynamically allocate all of the
			/// stupid commands and to clean them up I'd have to add them to my own stupid list of unique pointers
			/// and that would bloat this code to twice the size it already is so we're going to deal with it like this
			/// Someday I'll make a moveable command wrapper and better CommandGroup but today is not that day
			auton_command_ = std::make_unique<CommandGroup>();
			auto mode = auton_mode_.GetEnumValueOrDefault();
			auto defense = auton_defense_.GetEnumValueOrDefault();
			int position = auton_position_.GetValueOrDefault();

			auto& auton = GetSettings()["autonomous"];
			if(mode == NO_OP)
				return true;

			if(position == 1 || position == 2 || position == 4) grip_.set_left_bias(true);
			if(position == 3 || position == 5) grip_.set_left_bias(false);

			auton_command_->AddParallel(intake_.MakeIntakeBall(), 2);

			if(defense == PORTCULLIS)
			{
				auton_command_->AddSequential(swissCheez.MakeSetSwiss(Swiss::state_t::port_down), 1);
			}
			else if(defense == CHEVAL)
			{
				auton_command_->AddSequential(swissCheez.MakeSetSwiss(Swiss::state_t::horizontal), 1);
			}

			{
				auto& approach = auton["A_approach"];
				double speed = approach("speed").GetValueOrDefault<double>();
				double distance = approach("distance").GetValueOrDefault<double>();
				double timeout = approach("timeout").GetValueOrDefault<double>();

				// Approach the outerworks
				auton_command_->AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);
			}

			{
				auto& alignment = auton["B_alignment"];
				bool skip = alignment("skip").GetValueOrDefault<bool>();
				double timeout = alignment("timeout").GetValueOrDefault<double>();
				double speed_factor = alignment("speed_factor").GetValueOrDefault<double>();

				// Align with outerworks
				if(!skip)
				{
					auton_command_->AddSequential(drive_.MakeArcadeDrive(GetLocalValue<double>("Align/forward_output"),
														oi_.GetNullAxis(),
														speed_factor), timeout);
				}
			}

			if(defense == CHEVAL)
			{
				auto& swiss = auton["Swiss"];
				double timeout = swiss("timeout").GetValueOrDefault<double>();
				auton_command_->AddSequential(swissCheez.MakeSetSwiss(Swiss::state_t::cheval_down), timeout);
			}

			if(defense == CHEVAL)
			{
				auton_command_->AddParallel(swissCheez.MakeSetSwiss(Swiss::state_t::retract));
			}
			else if(defense == PORTCULLIS)
			{
				auton_command_->AddSequential(swissCheez.MakeSetSwiss(Swiss::state_t::retract));
			}

			{
				auto& cross = auton["Cross"];
				double speed = cross("speed").GetValueOrDefault<double>();
				double distance = cross("distance").GetValueOrDefault<double>();
				double timeout = cross("timeout").GetValueOrDefault<double>();

				// Drive over defense
				auton_command_->AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);
			}

			if(mode == IN)
			{
				if(defense == PORTCULLIS)
				{
					auton_command_->AddSequential(swissCheez.MakeSetSwiss(Swiss::state_t::retract));
				}

				return true;
			}

			if(mode == IN_OUT && defense != CHEVAL && defense != PORTCULLIS)
			{
				{
					auto& ret = auton["Return"];
					double speed = -fabs(ret("speed").GetValueOrDefault<double>());
					double distance = ret("distance").GetValueOrDefault<double>();
					double timeout = ret("timeout").GetValueOrDefault<double>();

					// Return over defense
					if(ret("leave_ball").GetValueOrDefault<bool>())
						auton_command_->AddSequential(intake_.MakePushBall());
					auton_command_->AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);

				}
				{
					auto& turn_around = auton["TurnAround"];
					double speed = turn_around("speed").GetValueOrDefault<double>();
					double left_revs = turn_around("left_revs").GetValueOrDefault<double>();
					double timeout = turn_around("timeout").GetValueOrDefault<double>();
					bool skip = turn_around("skip").GetValueOrDefault<bool>();

					if(!skip)
					{
						auton_command_->AddSequential(drive_.MakeTurn(speed, left_revs, false), timeout);
					}
				}

				return true;
			}
			else if(mode == IN_OUT)
			{

				{
					auto& turn_around = auton["TurnAround"];
					double speed = turn_around("speed").GetValueOrDefault<double>();
					double left_revs = turn_around("left_revs").GetValueOrDefault<double>();
					double timeout = turn_around("timeout").GetValueOrDefault<double>();
					bool skip = turn_around("skip").GetValueOrDefault<bool>();

					auton_command_->AddSequential(drive_.MakeTurn(speed, left_revs, false), timeout);
				}

				if(defense == CHEVAL)
				{
					auton_command_->AddParallel(swissCheez.MakeSetSwiss(Swiss::state_t::retract), 3);
				}

				{
					auto& ret = auton["Return"];
					double speed = fabs(ret("speed").GetValueOrDefault<double>());
					double distance = 0.0;
					if(defense == CHEVAL)
						distance = ret("short_distance").GetValueOrDefault<double>();
					else
						distance = ret("distance").GetValueOrDefault<double>();

					double timeout = ret("timeout").GetValueOrDefault<double>();
					auton_command_->AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);
				}

				if(defense == CHEVAL)
				{
					{
						auto& alignment = auton["B_alignment"];
						bool skip = alignment("skip").GetValueOrDefault<bool>();
						double timeout = alignment("timeout").GetValueOrDefault<double>();
						double speed_factor = alignment("speed_factor").GetValueOrDefault<double>();

						// Align with outerworks
						if(!skip)
						{
							auton_command_->AddSequential(drive_.MakeArcadeDrive(GetLocalValue<double>("Align/forward_output"),
															oi_.GetNullAxis(),
															speed_factor), timeout);
						}
					}

					auton_command_->AddParallel(swissCheez.MakeSetSwiss(Swiss::state_t::cheval_down), 3);

					{
						auto& ret = auton["Trickshot"];
						double timeout = ret("timeout").GetValueOrDefault<double>();
						double speed = fabs(ret("speed").GetValueOrDefault<double>());
						double distance = ret("distance").GetValueOrDefault<double>();

						auton_command_->AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);
					}

					auton_command_->AddSequential(swissCheez.MakeSetSwiss(Swiss::state_t::retract));
				}


			}
			else
			{
				{
					auto& approaches = auton["CourtyardApproaches"];
					double speed = approaches("speed").GetValueOrDefault<double>();
					double distance = approaches["from_" + std::to_string(position)]("distance").GetValueOrDefault<double>();
					double timeout = approaches("timeout").GetValueOrDefault<double>();

					// Move into a goal's line of sight
					this->Log(MessageData::STATUS, "Auton", "Courtyard Approach") << "distance: " << distance << " meters";
					auton_command_->AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);
				}

				{
					auto& turns = auton["Turns"];
					double speed = turns("speed").GetValueOrDefault<double>();
					double revs = turns["from_" + std::to_string(position)]("left_revs").GetValueOrDefault<double>();
					double timeout = turns("timeout").GetValueOrDefault<double>();

					if(revs < 0) speed *= -1;

					this->Log(MessageData::STATUS, "Auton", "Turn") << "left_revs: " << revs;

					// Turn towards the goal
					auton_command_->AddSequential(drive_.MakeTurn(speed, revs, false), timeout);
					auton_command_->AddSequential(camera_.MakeSetCamera(Camera::CamState_t::GOAL));
				}

				{
					auto& last_approach = auton["last_approach"];
					double speed = last_approach("speed").GetValueOrDefault<double>();
					double timeout = last_approach("timeout").GetValueOrDefault<double>();
					double distance = last_approach["from_" + std::to_string(position)]("distance").GetValueOrDefault<double>();

					if(mode == DEAD_HIGH_GOAL)
					{
						auton_command_->AddSequential(drive_.MakeDriveStraight(speed, distance, false), timeout);
					}
					else if(mode == AUTO_HIGH_GOAL)
					{
						auton_command_->AddSequential(drive_.MakeArcadeDrive(GetLocalValue<double>("Align/forward_output"),
																			GetLocalValue<double>("GRIP/turn_output"), speed, speed),
														timeout);
					}

					auton_command_->AddSequential(drive_.MakeDriveStraight(-0.1, 0.1, true), .1);
					auton_command_->AddSequential(shooter_.MakeSpinUp());
					auton_command_->AddSequential(new WaitCommand(1.0));
					auton_command_->AddSequential(new commands::Shoot(&intake_, &shooter_, 2.0, .25, 3.0));
					auton_command_->AddSequential(shooter_.MakeSpinDown());
				}
			}


		}

		return true;
	}

	void BindControls()
	{
		drive_.SetDefaultCommand(drive_.MakeArcadeDrive(oi_.GetArcadeForwardAxis(), oi_.GetArcadeTurnAxis(), 1.0, 1.0));
		swissCheez.SetDefaultCommand(swissCheez.MakeHoldSwiss());
		//camera_.SetDefaultCommand(camera_.MakeSetCamera(Camera::CamState_t::WHEEL));

		CommandGroup *intake_group = new CommandGroup;
		intake_group->AddParallel(camera_.MakeSetCamera(Camera::CamState_t::BALL));
		intake_group->AddParallel(intake_.MakeIntakeBall());
		commands.push_back(intake_group);
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/intake")));
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/B")));
		triggers.back()->CancelWhenActive(commands.back());
		commands.push_back(camera_.MakeSetCamera(Camera::CamState_t::BALL));
		triggers.back()->WhenActive(commands.back());
		commands.push_back(camera_.MakeSetCamera(Camera::CamState_t::WHEEL));
		triggers.back()->WhenInactive(commands.back());

		/*
		commands.push_back(camera_.MakeSetCamera(Camera::CamState_t::FRONT));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("Intake/holding_boulder")));
		triggers.back()->WhenActive(commands.back());
		*/

		auto push_ball_command = intake_.MakePushBall();

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/push")));
		commands.push_back(push_ball_command);
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/spin_up")));

		CommandGroup *spinup_group = new CommandGroup;
		spinup_group->AddParallel(shooter_.MakeSpinUp());
		spinup_group->AddParallel(oi_.MakeShootMode(&camera_, .8, .8));

		commands.push_back(spinup_group);
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/spin_down")));
		triggers.back()->CancelWhenActive(commands.back());

		commands.push_back(shooter_.MakeSpinDown());
		triggers.back()->WhenActive(commands.back());

		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/shoot")));
		commands.push_back(new commands::Shoot(&intake_, &shooter_, 0.5, .25, 1.5));
		triggers.back()->WhenActive(commands.back());
		triggers.back()->CancelWhenActive(spinup_group);

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

		CommandGroup *align_group = new CommandGroup;
		align_group->AddParallel(camera_.MakeSetCamera(Camera::CamState_t::GOAL));
		align_group->AddParallel(drive_.MakeArcadeDrive(oi_.GetArcadeForwardAxis(), GetLocalValue<double>("GRIP/turn_output")));
		commands.push_back(align_group);
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/driver_left/buttons/7")));
		triggers.back()->WhileActive(commands.back());

		// swiss
		for(int state = Camera::CamState_t::BACK; state < Camera::N_STATES; state++)
		{
			commands.push_back(camera_.MakeSetCamera(static_cast<Camera::CamState_t>(state)));
			triggers.push_back(new GenericTrigger(oi_.GetCameraStateButton(static_cast<Camera::CamState_t>(state))));
			triggers.back()->WhenActive(commands.back());
		}

		commands.push_back(swissCheez.MakeControlSwissVelocity(oi_.GetSwissAxis()));
		triggers.push_back(new GenericTrigger(oi_.GetControlSwissButton()));
		triggers.back()->WhileActive(commands.back());

		commands.push_back(swissCheez.MakeSetSwiss(Swiss::state_t::retract));
		triggers.push_back(new GenericTrigger(GetLocalValue<bool>("OI/xbox/buttons/L_TRIGGER")));
		triggers.back()->WhenActive(commands.back());

		commands.push_back(intake_.MakeControlIntake(oi_.GetIntakeAxis()));
		triggers.push_back(new GenericTrigger(oi_.GetControlIntakeButton()));
		triggers.back()->WhileActive(commands.back());
	}

	void BindDashboard()
	{

		std::string double_values[] =
		{"Robot/Align/left_error", "Robot/Align/right_error", "Robot/Align/forward_error", "Robot/Align/rotation_error",
		"Robot/Swiss/talon/output_current", "Robot/Swiss/talon/output_speed", "Robot/Swiss/talon/output_voltage",
		"Robot/CameraMount/position", "Robot/Shooter/error", "Robot/Shooter/top_roller_output_voltage",
		"Robot/Drive/distance",
		"Robot/GRIP/turn_output", "Robot/Shooter/target",
		"Robot/Intake/front_roller_ouput_voltage", "Robot/Intake/roller_error"};
		for(auto double_value : double_values)
			dashboard_.AddDashValue<double>(double_value);

		std::string boolean_values[] =
		{"Robot/Intake/holding_boulder", "Robot/Shooter/spunup", "Robot/GRIP/aligned"};
		for(auto bool_value : boolean_values)
			dashboard_.AddDashValue<bool>(bool_value);
	}

	// Disabled
	void DisabledInit() override
	{
		grip_.Configure();
		grip_.BringItDown();
	}

	void DisabledPeriodic() override
	{
		Scheduler::GetInstance()->Run();
	}


	// Teleop
	void TeleopInit() override
	{
		get_context().LoadConfig();
		if(auton_command_)
			auton_command_->Cancel();
		Configure();
		TestDriveInit();
		grip_.ItsLit();
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
		grip_.ItsLit();
		auton_command_->Start();
	}

	void AutonomousPeriodic() override
	{
		Scheduler::GetInstance()->Run();
	}

	void TestInit()
	{
		TestBoulderInit();
		drive_.SetMode(Drive::Mode_t::VBus);
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
			shooter_.SetMode(Shooter::Mode_t::VELOCITY);
		}
		else
		{
			shooter_.SetMode(Shooter::Mode_t::VBUS);
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
