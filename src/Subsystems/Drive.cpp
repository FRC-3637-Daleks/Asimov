/*
 * Drive.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: Edward
 */

#include "Drive.h"

#include "Utility/FunkyGet.h"

#include <cmath>
#include <memory>
#include <string>

namespace subsystems
{

std::string Drive::ModeToString(Mode_t mode)
{
	switch(mode)
	{
	case Mode_t::VBus:
		return "VBus";
	case Mode_t::Position:
		return "Position";
	case Mode_t::Velocity:
		return "Velocity";
	}
}

Drive::Drive(): dman::WPISystem("Drive"), talons_(nullptr), mode_(Mode_t::VBus),
		ticks_per_rev_(761), wheel_diameter_(0.3048), max_velocity_(120.0), wheel_revs_per_base_rev_(5000),
		allowable_error_(0.05), reset_timeout_(0.1) // 5 cm of wheel rotation
{
}

void Drive::initTalons()
{
	if(!is_initialized())
	{
		auto& ports = GetPortSpace("CAN");
		int left = ports("left");
		int left_slave = ports("left_slave");
		int right = ports("right");
		int right_slave = ports("right_slave");

		talons_ = std::make_unique<Talons>(left, left_slave,
										   right, right_slave);
		Log(dman::MessageData::INFO, "", "Subsystem") << "Port Config: left: " << left << ", " <<
				"left_slave: " << left_slave << ", right: " << right << ", right_slave: " << right_slave;
	}
}

bool Drive::configureMaster(CANTalon& master)
{
	std::cout << "Configuring a side" << std::endl;
	master.ConfigLimitMode(CANTalon::LimitMode::kLimitMode_SrxDisableSwitchInputs);
	master.ConfigNeutralMode(CANTalon::NeutralMode::kNeutralMode_Brake);
	master.SetFeedbackDevice(CANTalon::FeedbackDevice::QuadEncoder);
	SetTicksPerRev(get_ticks_per_rev());
	master.SetPosition(0.0);

	master.SetVoltageRampRate(50.0);

	if(GetSettings()["closed_loop_settings"]("use").GetValueOrDefault())
	{
		auto& settings = GetSettings()["closed_loop_settings"];
		if(!settings("P").is_empty()) master.SetP(settings("P").get_value());
		if(!settings("I").is_empty()) master.SetI(settings("I").get_value());
		if(!settings("D").is_empty()) master.SetD(settings("D").get_value());
		if(!settings("F").is_empty()) master.SetF(settings("F").get_value());
		if(!settings("I_Zone").is_empty()) master.SetIzone(settings("I_Zone").get_value());
		if(!settings("ramp_rate").is_empty()) master.SetCloseLoopRampRate(settings("ramp_rate").get_value());
	}

	return true;
}

bool Drive::configureBoth()
{
	if(is_initialized())
	{
		std::cout << "Configuring Drive" << std::endl;
		auto& settings = GetSettings();
		auto& ports = GetPortSpace("CAN");

		bool left_ret = configureMaster(talons_->left_);
		talons_->left_slave_.SetControlMode(CANTalon::ControlMode::kFollower);
		talons_->left_slave_.Set(ports("left"));
		talons_->left_.SetInverted(settings["left"]("invert_output").GetValueOrDefault());
		talons_->left_.SetSensorDirection(settings["left"]("invert_sensor").GetValueOrDefault());

		bool right_ret = configureMaster(talons_->right_);
		talons_->right_slave_.SetControlMode(CANTalon::ControlMode::kFollower);
		talons_->right_slave_.Set(ports("right"));
		talons_->right_.SetInverted(settings["right"]("invert_output").GetValueOrDefault());
		talons_->right_.SetSensorDirection(settings["right"]("invert_sensor").GetValueOrDefault());

		return left_ret && right_ret;
	}

	return false;
}

void Drive::doRegister()
{
	auto& can_ports = GetPortSpace("CAN");
	can_ports("right").SetDefault(1);
	can_ports("right_slave").SetDefault(2);
	can_ports("left").SetDefault(3);
	can_ports("left_slave").SetDefault(4);

	auto& settings = GetSettings();
	settings("ticks_per_revolution").SetDefault(get_ticks_per_rev());
	settings("wheel_diameter").SetDefault(get_wheel_diameter());
	settings("max_velocity").SetDefault(get_velocity_scale());
	settings("wheel_revolutions_per_chassis_pivot_revolution").SetDefault(get_wheel_revs_per_base_rev());
	settings("allowable_error").SetDefault(get_allowable_error());
	settings("reset_timeout").SetDefault(get_reset_timeout());

	auto& closed_loop_settings = settings["closed_loop_settings"];
	closed_loop_settings("use").SetDefault(false);
	closed_loop_settings("P").SetDefault(.1);
	closed_loop_settings("I").SetDefault(0.0);
	closed_loop_settings("D").SetDefault(0.0);
	closed_loop_settings("F").SetDefault(1.0);
	closed_loop_settings("I_Zone").SetDefault(2.5);
	closed_loop_settings("ramp_rate").SetDefault(10);

	settings["left"]("invert_output").SetDefault(false);
	settings["left"]("invert_sensor").SetDefault(false);
	settings["right"]("invert_output").SetDefault(true);
	settings["right"]("invert_sensor").SetDefault(true);

	GetLocalValue<double>("distance").Initialize(std::make_shared<dman::FunkyGet<double> > ([this]()
		{
					if(is_initialized())
						return GetDistance();
		}));

}

bool Drive::doConfigure()
{
	if(!is_initialized())
		initTalons();

	auto& settings = GetSettings();
	SetTicksPerRev(settings("ticks_per_revolution").GetValueOrDefault());
	SetWheelDiameter(settings("wheel_diameter").GetValueOrDefault());
	SetVelocityScale(settings("max_velocity").GetValueOrDefault());
	SetWheelRevsPerBaseRev(settings("wheel_revolutions_per_chassis_pivot_revolution").GetValueOrDefault());
	SetAllowableError(settings("allowable_error").GetValueOrDefault());
	SetResetTimeout(settings("reset_timeout").GetValueOrDefault<double>());

	Log(dman::MessageData::STATUS, "Drive", "Subsystem") << "Reset Timeout: " << get_reset_timeout();

	ResetPosition();

	return configureBoth();
}

void Drive::SetMode(Mode_t m)
{
	if(!is_initialized())
		return;

	if(m == mode_)
		return;
	else
	{
		mode_ = m;
		std::cout << "Setting mode to " << ModeToString(mode_) << std::endl;
		setModeMaster(talons_->left_);
		setModeMaster(talons_->right_);
	}
}

void Drive::setModeMaster(CANTalon &master)
{
	if(get_mode() == Mode_t::Position)
	{
		master.SetControlMode(CANTalon::ControlMode::kPosition);
		master.SelectProfileSlot(ModePIDSlot_t::PositionPID);
	}
	else if(get_mode() == Mode_t::Velocity)
	{
		master.SetControlMode(CANTalon::ControlMode::kSpeed);
		master.SelectProfileSlot(ModePIDSlot_t::VelocityPID);
	}
	else if(get_mode() == Mode_t::VBus)
	{
		master.SetControlMode(CANTalon::ControlMode::kPercentVbus);
	}
}

void Drive::SetTicksPerRev(Ticks_t val)
{
	ticks_per_rev_ = val;
	if(is_initialized())
	{
		talons_->left_.ConfigEncoderCodesPerRev(ticks_per_rev_);
		talons_->right_.ConfigEncoderCodesPerRev(ticks_per_rev_);
	}
}

void Drive::SetWheelDiameter(Meters_t val)
{
	wheel_diameter_ = val;
}

void Drive::SetVelocityScale(MetersPerSecond_t max_velocity)
{
	max_velocity_ = max_velocity;
}

void Drive::SetWheelRevsPerBaseRev(double rate)
{
	wheel_revs_per_base_rev_= rate;
}

void Drive::SetAllowableError(double allow)
{
	allowable_error_ = allow;
}

Drive::Meters_t Drive::GetDistance() const
{
	return (GetLeftRevs())* get_wheel_circumference();
}

double Drive::GetRotation() const
{
	return (GetLeftRevs() - GetRightRevs()) / 2.0 * get_wheel_revs_per_base_rev();
}

double Drive::GetLeftRevs() const
{
	if(is_initialized() && reset_timer_.HasPeriodPassed(get_reset_timeout()))
	{
		return talons_->left_.GetPosition();
	}
	else
		return 0.0;
}

double Drive::GetRightRevs() const
{
	if(is_initialized() && reset_timer_.HasPeriodPassed(get_reset_timeout()))
	{
		return talons_->right_.GetPosition();
	}
	else
		return 0.0;
}

double Drive::GetLeftRPM() const
{
	if(is_initialized())
		return talons_->left_.GetSpeed();
	else
		return 0.0;
}

double Drive::GetRightRPM() const
{
	if(is_initialized())
		return talons_->right_.GetSpeed();
	else
		return 0.0;
}

double Drive::GetLeftSetPointRPM() const
{
	if(is_initialized())
		return talons_->left_.GetSetpoint();
	else
		return 0.0;
}

double Drive::GetRightSetPointRPM() const
{
	if(is_initialized())
		return talons_->right_.GetSetpoint();
	else
		return 0.0;
}

Drive::MetersPerSecond_t Drive::GetVelocity() const
{
	return (GetLeftRPM() + GetRightRPM()) / 2.0 * get_wheel_circumference();
}

double Drive::GetBaseRPM() const
{
	return (GetLeftRPM() - GetRightRPM()) / 2.0 * get_wheel_revs_per_base_rev();
}

void Drive::ResetPosition()
{
	if(is_initialized())
	{
		talons_->left_.SetPosition(0.0);
		talons_->right_.SetPosition(0.0);
		reset_timer_.Reset();
		reset_timer_.Start();
	}
}

bool Drive::GoToPosition(Meters_t pos)
{
	SetMode(Mode_t::Position);
	if(is_initialized())
	{
		talons_->left_.Set(pos * get_wheel_circumference(), 3);
		talons_->right_.Set(pos * get_wheel_circumference(), 3);
		CANJaguar::UpdateSyncGroup(3);

		if(fabs(GetDistance() - pos) < get_allowable_error())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Drive::TurnToRotation(double revolutions)
{
	SetMode(Mode_t::Position);
	if(is_initialized())
	{
		talons_->left_.Set(revolutions * get_wheel_revs_per_base_rev(), 3);
		talons_->right_.Set(revolutions * get_wheel_revs_per_base_rev(), 3);
		CANJaguar::UpdateSyncGroup(3);

		if(fabs(GetRotation() - revolutions) * get_wheel_revs_per_base_rev() <
				get_allowable_error() / get_wheel_diameter())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void Drive::TankDrive(double left, double right)
{
	if(get_mode() == Mode_t::Position)
		return;

	if(is_initialized())
	{
		if(left > 1.0) left = 1.0;
		if(left < -1.0) left = -1.0;
		if(right > 1.0) right = 1.0;
		if(right < -1.0) right = -1.0;

		if(get_mode() == Mode_t::Velocity)
		{
			left *= get_velocity_scale();
			right *= get_velocity_scale();
		}


		talons_->left_.Set(left, 3);
		talons_->right_.Set(right, 3);
		CANJaguar::UpdateSyncGroup(3);
	}
}

void Drive::Stop()
{
	TankDrive(0.0, 0.0);
}

void Drive::ArcadeDrive(double y, double rotation)
{
	if(y > 1.0) y = 1.0;
	if(y < -1.0) y = -1.0;
	if(rotation > 1.0) rotation = 1.0;
	if(rotation < -1.0) rotation = -1.0;
	TankDrive(y - rotation, y + rotation);
}

}
