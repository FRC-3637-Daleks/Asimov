/*
 * Drive.cpp
 *
 *  Created on: Feb 1, 2016
 *      Author: Edward
 */

#include "Drive.h"

#include <cmath>

namespace subsystems
{

Drive::Drive(): talons_(nullptr), mode_(Mode_t::VBus),
		ticks_per_rev_(1024), wheel_diameter_(0.3048), max_velocity_(10.0), wheel_revs_per_base_rev_(5000),
		allowable_error_(0.05) // 5 cm of wheel rotation
{
}

void Drive::SetMode(Mode_t m)
{
	if(m == mode_)
		return;
	else
	{
		mode_ = m;
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

void Drive::ArcadeDrive(double y, double rotation)
{
	if(y > 1.0) y = 1.0;
	if(y < -1.0) y = -1.0;
	if(rotation > 1.0) rotation = 1.0;
	if(rotation < -1.0) rotation = -1.0;
	ArcadeDrive(y + rotation, y - rotation);
}

}
