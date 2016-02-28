#include "WPILib.h"
#include "Subsystems/Swiss.h"
#include "Subsystems/Align.h"
using namespace subsystems;

class Asimov: public IterativeRobot
{
public:
	Asimov(): swissCheez(7, 0, 0, 0), derp(2), sensors()
	{}
	Swiss swissCheez;
	Joystick derp;
	Align sensors;

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
		StateThing();
		SmartDashboard::PutNumber("Forward Error", sensors.ForwardError());
		SmartDashboard::PutNumber("Rotation Error", sensors.RotationError());
		SmartDashboard::PutNumber("Left Error", sensors.LeftError());
		SmartDashboard::PutNumber("Right Error", sensors.RightError());

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
		TestSwiss();
	}

	void StateThing(){
		if(derp.GetRawButton(5)){
			swissCheez.SetMode(Swiss::mode_t::pos);
		}
		if(derp.GetRawButton(7)){
			swissCheez.SetMode(Swiss::mode_t::velocity);
		}
		if(derp.GetRawButton(8)){
			swissCheez.SetMode(Swiss::mode_t::vbus);
		}
	}

	void TestSwiss()
	{
		StateThing();
		if (swissCheez.GetMode()==Swiss::mode_t::pos) {
			if(derp.GetRawButton(1)){
				swissCheez.MinHeight();
			}
			if(derp.GetRawButton(2)){
				swissCheez.MaxHeight();
			}
			if(derp.GetRawButton(3)){
				swissCheez.LowerCheval();
			}
			if(derp.GetRawButton(4)){
				swissCheez.LiftDoor();
			}
			if(derp.GetRawButton(6)){
				swissCheez.LowerPort();
			}

		} else {
			swissCheez.SetVelocity(derp.GetRawAxis(1), false);
		}
	}
};

START_ROBOT_CLASS(Asimov)

