/*
 * Drive.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: Edward
 */

#include "Drive.h"

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

Drive::Drive(): Subsystem("Drive"), talons_(nullptr), mode_(Mode_t::VBus),
		ticks_per_rev_(761), wheel_diameter_(0.3048), max_velocity_(120.0), wheel_revs_per_base_rev_(5000),
		allowable_error_(0.05) // 5 cm of wheel rotation
{
}

bool Drive::Initialize()
{
	talons_ = std::make_unique<Talons>(3, 4, 1, 2);

	return configureBoth();
}

bool Drive::Configure()
{
	return configureBoth();
}

void Drive::SetMode(Mode_t m)
{
	if(m == mode_)
		return;
	else
	{
		mode_ = m;
		std::cout << "Setting mode to " << ModeToString(mode_) << std::endl;
		configureBoth();
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

Drive::Meters_t Drive::GetDistance() const
{
	return (GetLeftRevs() + GetRightRevs()) / 2.0 * get_wheel_circumference();
}

double Drive::GetRotation() const
{
	return (GetLeftRevs() - GetRightRevs()) / 2.0 * get_wheel_revs_per_base_rev();
}

double Drive::GetLeftRevs() const
{
	if(is_initialized())
		return talons_->left_.GetPosition();  //<  Current understanding is that this returns revs
	else
		return 0.0;
}

double Drive::GetRightRevs() const
{
	if(is_initialized())
		return talons_->right_.GetPosition();  //< See GetLeftRevs
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
	talons_->left_.StopMotor();
	talons_->right_.StopMotor();
}

void Drive::ArcadeDrive(double y, double rotation)
{
	if(y > 1.0) y = 1.0;
	if(y < -1.0) y = -1.0;
	if(rotation > 1.0) rotation = 1.0;
	if(rotation < -1.0) rotation = -1.0;
	TankDrive(y - rotation, y + rotation);
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

	return true;
}

bool Drive::configureBoth()
{
	if(is_initialized())
	{
		std::cout << "Configuring Drive" << std::endl;
		bool left_ret = configureMaster(talons_->left_);
		talons_->left_slave_.SetControlMode(CANTalon::ControlMode::kFollower);
		talons_->left_slave_.Set(3);
		talons_->left_.SetInverted(true);
		talons_->left_.SetSensorDirection(true);

		bool right_ret = configureMaster(talons_->right_);
		talons_->right_slave_.SetControlMode(CANTalon::ControlMode::kFollower);
		talons_->right_slave_.Set(1);
		talons_->right_.SetInverted(false);
		talons_->right_.SetSensorDirection(true);

		return left_ret && right_ret;
	}

	return false;
}

}
