#include "WPILib.h"
#include "Subsystems/Swiss.h"

using namespace subsystems;

class Asimov: public IterativeRobot
{
public:
	Asimov(): derp(2)
	{}
	Swiss swissCheez;
	Joystick derp;

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
				swissCheez.SetState(Swiss::state_t::retract);
			}
			if(derp.GetRawButton(2)){
				swissCheez.SetState(Swiss::state_t::horizontal);
			}
			if(derp.GetRawButton(3)){
				swissCheez.SetState(Swiss::state_t::cheval_down);
			}
			if(derp.GetRawButton(4)){
				swissCheez.SetState(Swiss::state_t::port_down);
			}

		} else {
			swissCheez.SetVelocity(derp.GetRawAxis(5), false);
		}
	}
};

START_ROBOT_CLASS(Asimov)

