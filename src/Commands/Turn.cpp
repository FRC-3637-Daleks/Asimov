/*
 * Turn.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: Edward
 */


#include "Turn.h"

#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

// STD Includes
#include <cmath>

namespace commands
{

Turn::Turn(Drive_t *drive, double velocity, double length, bool in_seconds):
		drive_(drive), velocity_(velocity), length_(fabs(length)), type_(in_seconds? SECONDS:REVOLUTIONS)
{
	Requires(drive_);
	SetInterruptible(true);

	if(velocity > 1.0) velocity = 1.0;
	if(velocity < -1.0) velocity = -1.0;

	if(type_ == SECONDS) SetTimeout(length_);
}

Turn::Turn(Drive_t *drive, double velocity):
		drive_(drive), velocity_(velocity), length_(0.0), type_(FOREVER)
{
	Requires(drive_);
	SetInterruptible(true);

	if(velocity > 1.0) velocity = 1.0;
	if(velocity < -1.0) velocity = -1.0;
}

void Turn::Initialize()
{
	if(drive_ == nullptr)
	{
		Cancel();
		dman::TextLog::Log(dman::MessageData::ERR, dman::SystemData("Drive", "Turn", "Command")) <<
				"Initialize Failed";
		return;
	}

	drive_->ResetPosition();
	drive_->TankDrive(-velocity_, velocity_);
	drive_->Log(dman::MessageData::STATUS, "Turn", "Command") << "DriveStraight initialized, driving at: " <<
			velocity_;
}

void Turn::Execute()
{
	if(drive_ == nullptr)
	{
		return;
	}
}

bool Turn::IsFinished()
{
	if(drive_ == nullptr)
		return true;

	if(type_ == FOREVER)
		return false;

	return IsTimedOut() || fabs(drive_->GetLeftRevs()) > length_;
}

void Turn::End()
{
	if(drive_ == nullptr)
		return;

	drive_->Stop();
	drive_->Log(dman::MessageData::STATUS, "Turn", "Command") << "Ended, stopping drive";
}

void Turn::Interrupted()
{
	drive_->Log(dman::MessageData::INFO, "Turn", "Command") << "Interrupted";
	End();
}

}



