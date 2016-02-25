/*
 * DriveStraight.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: Edward
 */

#include "DriveStraight.h"

// STD Includes
#include <cmath>

namespace commands
{

DriveStraight::DriveStraight(Drive_t *drive, double velocity, double length, bool in_seconds):
		drive_(drive), velocity_(velocity), length_(fabs(length)), type_(in_seconds? SECONDS:METERS)
{
	Requires(drive_);
	SetInterruptible(true);

	if(velocity > 1.0) velocity = 1.0;
	if(velocity < -1.0) velocity = -1.0;

	if(type_ == SECONDS) SetTimeout(length_);
}

DriveStraight::DriveStraight(Drive_t *drive, double velocity):
		drive_(drive), velocity_(velocity), length_(0.0), type_(FOREVER)
{
	Requires(drive_);
	SetInterruptible(true);

	if(velocity > 1.0) velocity = 1.0;
	if(velocity < -1.0) velocity = -1.0;
}

void DriveStraight::Initialize()
{
	if(drive_ == nullptr)
	{
		Cancel();
		return;
	}

	drive_->TankDrive(velocity_, velocity_);
}

bool DriveStraight::IsFinished()
{
	if(drive_ == nullptr)
		return true;

	if(type_ == FOREVER)
		return false;

	return IsTimedOut() || fabs(drive_->GetDistance()) > length_;
}

void DriveStraight::End()
{
	if(drive_ == nullptr)
		return;

	drive_->Stop();
}

void DriveStraight::Interrupted()
{
	End();
}

}
