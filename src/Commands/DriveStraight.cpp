/*
 * DriveStraight.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: Edward
 */

#include "DriveStraight.h"

#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

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
		dman::TextLog::Log(dman::MessageData::ERR, dman::SystemData("Drive", "DriveStraight", "Command")) <<
				"Initialize Failed";
		return;
	}

	drive_->ResetPosition();
	drive_->TankDrive(velocity_, velocity_);
	drive_->Log(dman::MessageData::STATUS, "DriveStraight", "Command") << "DriveStraight initialized, driving at: " <<
			velocity_;
}

void DriveStraight::Execute()
{
	if(drive_ == nullptr)
	{
		return;
	}
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
	drive_->Log(dman::MessageData::STATUS, "DriveStraight", "Command") << "Ended, stopping drive";
}

void DriveStraight::Interrupted()
{
	drive_->Log(dman::MessageData::INFO, "DriveStraight", "Command") << "Interrupted";
	End();
}

}
