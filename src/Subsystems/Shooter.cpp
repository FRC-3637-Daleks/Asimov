#include "Shooter.h"
#include "ctre/Phoenix.h"
#include "Commands/SpinUp.h"
#include "Commands/SpinDown.h"
#include "Commands/VarSpinUp.h"
#include "Utility/FunkyGet.h"
#include <math.h>

namespace subsystems
{

using namespace dman;
using SpinUp = commands::SpinUp;
using SpinDown = commands::SpinDown;
using VarSpinUp = commands::VarSpinUp;

// Constructor:
Shooter::Shooter() : dman::WPISystem("Shooter")
{
	top_roller_ = NULL; // leave null until configuration

	state_ = State_t::OFF;
	mode_ = Mode_t::VBUS;

	max_velocity_ = 2200000;
	allowed_error_ = 0.1;
	shoot_percent_ = .93;
}

// Destructor:
Shooter::~Shooter()
{
	delete top_roller_;
}

// Dalek manager config functions:
void Shooter::doRegister()
{
	// CAN port settings
	auto& can_ports = GetPortSpace("CAN");
	can_ports("shooter_roller").SetDefault(6);

	// Instance variable settings
	auto& settings = GetSettings();
	settings("max_velocity").SetDefault(max_velocity_);
	settings("allowed_error").SetDefault(allowed_error_);
	settings("shoot_percent").SetDefault(shoot_percent_);

	// PID settings
	auto& closed_loop_settings = settings["closed_loop_settings"];
	closed_loop_settings("use").SetDefault(false);
	closed_loop_settings("P").SetDefault(.1);
	closed_loop_settings("I").SetDefault(0.0);
	closed_loop_settings("D").SetDefault(0.0);
	closed_loop_settings("F").SetDefault(1.0);
	closed_loop_settings("I_Zone").SetDefault(2.5);
	closed_loop_settings("ramp_rate").SetDefault(10);
	closed_loop_settings("init_closed_loop").SetDefault(false);
	closed_loop_settings("voltage_compensation_rate").SetDefault(1.0);

	// Inversion settings
	settings("invert_output").SetDefault(false);
	settings("invert_sensor").SetDefault(false);
	settings("reverse_closed").SetDefault(false);
	settings("encoder_codes_per_rev").SetDefault(2);

	// Value store functions
	GetLocalValue<double>("top_roller_temp").Initialize(std::make_shared<FunkyGet<double> > ([this]() -> double
			{
				if(top_roller_)
					return top_roller_->GetTemperature();
				return 0.0;
			}));
	GetLocalValue<double>("top_roller_output_voltage").Initialize(std::make_shared<FunkyGet<double> > ([this]() -> double
			{
				if(top_roller_)
					return top_roller_->GetMotorOutputVoltage();
				return 0.0;
			}));
	GetLocalValue<double>("top_roller_output_current").Initialize(std::make_shared<FunkyGet<double> > ([this]() -> double
			{
				if(top_roller_)
					return top_roller_->GetOutputCurrent();
				return 0.0;
			}));
	GetLocalValue<double>("error").Initialize(std::make_shared<FunkyGet<double> >([this] () -> double
			{
				if(top_roller_ && top_roller_->GetClosedLoopTarget(0) != 0)
 					return (double)top_roller_->GetClosedLoopError(0);
				return 0.0;
			}));
	GetLocalValue<double>("target").Initialize(std::make_shared<FunkyGet<double> >([this] () -> double
			{
				if(top_roller_)
					return (double)top_roller_->GetClosedLoopTarget(0);
				return 0.0;
			}));
	GetLocalValue<bool>("spunup").Initialize(std::make_shared<FunkyGet<bool> > ([this] ()
			{
				if(top_roller_)
					return IsAllowable()/* && state_ == Shooter::State_t::SPUNUP*/;
				return false;
			}));
}

bool Shooter::doConfigure()
{
	Log(dman::MessageData::INFO, "", "Subsystem") << "Configuring Shooter";

	// Initialize roller to CAN port
	auto& can_ports = GetPortSpace("CAN");
	if (top_roller_ == NULL)
		top_roller_ = new WPI_TalonSRX(can_ports("shooter_roller").GetValueOrDefault());

	// Configure variable values
	auto& settings = GetSettings();
	SetMaxVelocity(settings("max_velocity").GetValueOrDefault());
	SetAllowedError(settings("allowed_error").GetValueOrDefault());
	SetShootPercent(settings("shoot_percent").GetValueOrDefault());

	// Configure closed loop settings
	auto& closed_loop_settings = settings["closed_loop_settings"];
	if (closed_loop_settings("init_closed_loop").GetValueOrDefault<bool>() == false)
	{
		this->SetMode(Mode_t::VBUS);
	}
	else
	{
		this->SetMode(Mode_t::VELOCITY);
	}

	if (closed_loop_settings("use").GetValueOrDefault() == true)
	{
		top_roller_->Config_kP(0, closed_loop_settings("P").GetValueOrDefault(), 10);
		top_roller_->Config_kI(0, closed_loop_settings("I").GetValueOrDefault(), 10);
		top_roller_->Config_kD(0, closed_loop_settings("D").GetValueOrDefault(), 10);
		top_roller_->Config_kF(0, closed_loop_settings("F").GetValueOrDefault(), 10);
		top_roller_->Config_IntegralZone(0, closed_loop_settings("I_Zone").GetValueOrDefault(), 10);
		top_roller_->ConfigClosedloopRamp(closed_loop_settings("ramp_rate").GetValueOrDefault(), 10);
	}

	// Configure roller settings
	top_roller_->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 100);

	top_roller_->SetInverted(settings("invert_output").GetValueOrDefault());
	top_roller_->SetSensorPhase(settings("invert_sensor").GetValueOrDefault());

	top_roller_->ConfigNominalOutputForward(0.0, 10);
	top_roller_->ConfigNominalOutputReverse(0.0, 10);
	top_roller_->ConfigPeakOutputForward(1.0, 10);
	top_roller_->ConfigPeakOutputReverse(0.0, 10);

	SetMode(mode_);

	return true;
}

// Main functions:

void Shooter::SpinUp(double speed)
{
	if (mode_ == Mode_t::VELOCITY)
	{
		top_roller_->Set(max_velocity_ * speed);
	}
	else if (mode_ == Mode_t::VBUS)
	{
		top_roller_->Set(speed);
	}
}

void Shooter::SpinDown()
{
	top_roller_->Set(0.0);
}

void Shooter::EmergencyStop()
{
	//top_roller_->StopMotor();
	SpinDown();
}

double Shooter::GetMaxVelocity() const
{
	return max_velocity_;
}

void Shooter::SetMaxVelocity(double max_velocity)
{
	max_velocity_ = max_velocity;
}

double Shooter::GetSpeed() const
{
	return top_roller_->GetSensorCollection().GetQuadratureVelocity();
}

// Error functions:
double Shooter::GetErr() const
{
	return (double)top_roller_->GetClosedLoopError(0);
}

void Shooter::SetAllowedError(double err)
{
	allowed_error_ = err;
}

double Shooter::GetAllowedError() const
{
	return allowed_error_;
}

bool Shooter::IsAllowable() const
{
	return (fabs(GetErr()) < fabs(allowed_error_));
}

// State functions:
Shooter::State_t Shooter::GetState() const
{
	return state_;
}

void Shooter::SetState(State_t state)
{
	state_ = state;
}

// Control mode functions:
Shooter::Mode_t Shooter::GetMode() const
{
	return mode_;
}

void Shooter::SetMode(Mode_t mode)
{
	mode_ = mode;
	if(!top_roller_)
		return;
}

void Shooter::SetShootPercent(double percent)
{
	shoot_percent_ = percent;
}

double Shooter::GetShootPercent() const
{
	return shoot_percent_;
}

// Command functions:
SpinUp* Shooter::MakeSpinUp(double speed)
{
	return new commands::SpinUp(this, speed);
}

VarSpinUp* Shooter::MakeVarSpinUp(VarSpinUp::Input_t modifier)
{
	return new commands::VarSpinUp(this, std::move(modifier));
}

SpinDown* Shooter::MakeSpinDown()
{
	return new commands::SpinDown(this);
}

} // end namespace subsystems
