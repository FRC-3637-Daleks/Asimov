#include "Intake.h"
#include "Commands/IntakeBall.h"
#include "Commands/ControlIntake.h"
#include "Commands/PushBall.h"
#include "Utility/FunkyGet.h"
#include <math.h>

/**
 * Subsystems namespace with implementation
 * of Intake class.
 */
namespace subsystems
{
using namespace dman;
using IntakeBall = commands::IntakeBall;
using PushBall = commands::PushBall;
using ControlIntake = commands::ControlIntake;

// Constructor:
Intake::Intake() : dman::WPISystem("Intake")
{
	roller_ = NULL; // keep null until configure time
	detector_ = nullptr;

	// Default values:
	intake_speed_ = 0.5;
	push_speed_ = -1.0;

	state_ = State_t::OFF;
	mode_ = Mode_t::VBUS;

	max_velocity_ = 1000.0;
	allowed_error_ = 100;
	shoot_velocity_ = 1.0;
}

// Destructor:
Intake::~Intake()
{
	delete roller_;
	delete detector_;
}

// Dalek Manager config functions:
void Intake::doRegister()
{
	// CAN port settings:
	auto& can_ports = GetPortSpace("CAN");
	can_ports("intake_roller").SetDefault(5);

	GetPortSpace("DIO")("boulder_detector").SetDefault(0);

	// Instance variable settings
	auto& settings = GetSettings();
	settings("intake_speed").SetDefault(intake_speed_);
	settings("push_speed").SetDefault(push_speed_);
	settings("max_velocity").SetDefault(max_velocity_);
	settings("allowed_error").SetDefault(allowed_error_);
	settings("shoot_velocity").SetDefault(shoot_velocity_);

	// PID Settings
	auto& closed_loop_settings = settings["closed_loop_settings"];
	closed_loop_settings("use").SetDefault(false);
	closed_loop_settings("P").SetDefault(.1);
	closed_loop_settings("I").SetDefault(0.0);
	closed_loop_settings("D").SetDefault(0.0);
	closed_loop_settings("F").SetDefault(1.0);
	closed_loop_settings("I_Zone").SetDefault(2.5);
	closed_loop_settings("ramp_rate").SetDefault(10);

	// Inversion settings
	settings("invert_output").SetDefault(false);
	settings("invert_sensor").SetDefault(false);
	settings("encoder_codes_per_rev").SetDefault(8);

	// Value store functions
	GetLocalValue<double>("front_roller_temp").Initialize(std::make_shared<FunkyGet<double> >([this]() -> double
			{
				if(roller_)
					return roller_->GetTemperature();
				else
					return 0.0;
			}));
	GetLocalValue<double>("front_roller_ouput_voltage").Initialize(std::make_shared<FunkyGet<double> > ([this] () -> double
			{
				if(roller_)
					return roller_->GetMotorOutputVoltage();
				else
					return 0.0;
			}));
	GetLocalValue<double>("front_roller_ouput_current").Initialize(std::make_shared<FunkyGet<double> > ([this] () -> double
			{
				if(roller_)
					return roller_->GetOutputCurrent();
				else
					return 0.0;
			}));
	GetLocalValue<double>("roller_error").Initialize(std::make_shared<FunkyGet<double> >([this] () -> double
			{
				if(roller_)
					return GetErr();
				else
					return 0.0;
			}));
	GetLocalValue<bool>("holding_boulder").Initialize(std::make_shared<FunkyGet<bool> > ([this] ()
			{
				if(detector_)
					return this->CheckSwitch();
				else
					return false;
			}));
}

bool Intake::doConfigure()
{
	Log(dman::MessageData::STATUS, "", "Subsystem") << "Configuring Intake";

	// Initialize roller to CAN port
	auto& can_ports = GetPortSpace("CAN");
	if (roller_ == NULL)
		roller_ = new WPI_TalonSRX(can_ports("intake_roller").GetValueOrDefault());

	if(detector_ == NULL)
		detector_ = new DigitalInput(GetPortSpace("DIO")("boulder_detector").GetValueOrDefault());

	// Configure variable values
	auto& settings = GetSettings();
	SetIntakeSpeed(settings("intake_speed").GetValueOrDefault());
	SetPushSpeed(settings("push_speed").GetValueOrDefault());
	SetMaxVelocity(settings("max_velocity").GetValueOrDefault());
	SetAllowedError(settings("allowed_error").GetValueOrDefault());
	SetShootVelocity(settings("shoot_velocity").GetValueOrDefault());

	// Configure closed loop settings
	auto& closed_loop_settings = settings["closed_loop_settings"];
	if (closed_loop_settings("use").GetValueOrDefault() == true)
	{
		roller_->Config_kP(0, closed_loop_settings("P").GetValueOrDefault(), 10);
		roller_->Config_kI(0, closed_loop_settings("I").GetValueOrDefault(), 10);
		roller_->Config_kD(0, closed_loop_settings("D").GetValueOrDefault(), 10);
		roller_->Config_kF(0, closed_loop_settings("F").GetValueOrDefault(), 10);
		roller_->Config_IntegralZone(0, closed_loop_settings("I_Zone").GetValueOrDefault(), 10);
		roller_->ConfigClosedloopRamp(closed_loop_settings("ramp_rate").GetValueOrDefault(), 10);
	}

	// Configure roller settings
	roller_->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 10);

	roller_->SetInverted(settings("invert_output").GetValueOrDefault());
	roller_->SetSensorPhase(settings("invert_sensor").GetValueOrDefault());

	roller_->ConfigNominalOutputForward(0.0, 10);
	roller_->ConfigNominalOutputReverse(0.0, 10);
	roller_->ConfigPeakOutputForward(1.0, 10);
	roller_->ConfigPeakOutputReverse(-1.0, 10);

	SetMode(mode_);

	return true;
}

bool Intake::CheckSwitch() const
{
	return !(detector_->Get());
}

void Intake::TakeBall(bool check)
{
	if (CheckSwitch() && check)
		Stop();
	else
		SetSpeed(intake_speed_);
}

void Intake::OutakeBall()
{
	SetSpeed(push_speed_);
}

void Intake::ShootBall()
{
	SetSpeed(shoot_velocity_);
}

void Intake::SetSpeed(double speed)
{
	if (mode_ == Mode_t::VELOCITY)
	{
		roller_->Set(ControlMode::Velocity, max_velocity_ * speed);
	}
	else if (mode_ == Mode_t::VBUS)
	{
		roller_->Set(ControlMode::PercentOutput, speed);
	}
}

double Intake::GetSpeed() const
{
	return roller_->Get();
}

void Intake::Stop()
{
	SetSpeed(0.0);
}

void Intake::SetIntakeSpeed(double intake_speed)
{
	intake_speed_ = intake_speed;
}

void Intake::SetPushSpeed(double push_speed)
{
	push_speed_ = push_speed;
}

double Intake::GetIntakeSpeed() const
{
	return intake_speed_;
}

double Intake::GetPushSpeed() const
{
	return push_speed_;
}

double Intake::GetMaxVelocity() const
{
	return max_velocity_;
}

void Intake::SetMaxVelocity(double max_velocity)
{
	max_velocity_ = max_velocity;
}

void Intake::SetShootVelocity(double shoot_velocity)
{
	shoot_velocity_ = shoot_velocity;
}

double Intake::GetShootVelocity() const
{
	return shoot_velocity_;
}

double Intake::GetCurrentPosition() const
{
	return roller_->GetSensorCollection().GetQuadraturePosition();
}

// Error functions:
double Intake::GetErr() const
{
	return roller_->GetClosedLoopError(0);
}

void Intake::SetAllowedError(double err)
{
	allowed_error_ = err;
}

double Intake::GetAllowedError() const
{
	return allowed_error_;
}

bool Intake::IsAllowable() const
{
	return (fabs(GetErr()) > fabs(allowed_error_));
}

// State functions:
void Intake::SetState(State_t state)
{
	state_ = state;
}

Intake::State_t Intake::GetState() const
{
	return state_;
}

// Control mode functions:
void Intake::SetMode(Mode_t mode)
{
	mode_ = mode;
	if(!roller_)
		return;
}

Intake::Mode_t Intake::GetMode() const
{
	return mode_;
}

// Command functions:
IntakeBall* Intake::MakeIntakeBall()
{
	return new IntakeBall(this);
}

PushBall* Intake::MakePushBall()
{
	return new PushBall(this);
}

ControlIntake * Intake::MakeControlIntake(dman::ValueStore::Value<double> input)
{
	return new ControlIntake(this, std::move(input));
}

} // end namespace subsystems
