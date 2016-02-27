/*
 * ArcadeDrive.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: Edward
 */


#include "ArcadeDrive.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

namespace commands
{

using namespace dman;

ArcadeDrive::ArcadeDrive(Drive_t *drive, Input_t forward_control, Input_t turn_control,
		double forward_multiplier, double turn_multiplier):
		drive_(drive),
		forward_control_(std::move(forward_control)),
		turn_control_(std::move(turn_control)),
		forward_multiplier_(forward_multiplier),
		turn_multiplier_(turn_multiplier)
{
	Requires(drive);
}

void ArcadeDrive::Initialize()
{
	if(forward_control_.initialized() && turn_control_.initialized() && drive_ != nullptr)
	{
		TextLog::Log(MessageData(MessageData::INFO), SystemData("Drive", "ArcadeDrive", "Command")) <<
				"Initializing ArcadeDrive with initial forward input: " << forward_control_.GetValue() <<
				" and initial turn input: " << turn_control_.GetValue();
	}
	else
	{
		TextLog::Log(MessageData(MessageData::ERR), SystemData("Drive", "ArcadeDrive", "Command")) <<
				"Failed to Initialize ArcadeDrive";
		Cancel();
	}
}

void ArcadeDrive::Execute()
{
	drive_->ArcadeDrive(forward_control_.GetValue() * forward_multiplier_,
						turn_control_.GetValue() * turn_multiplier_);
}

void ArcadeDrive::End()
{
	drive_->Stop();
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Drive", "ArcadeDrive", "Command")) <<
			"Ending ArcadeDrive";
}

void ArcadeDrive::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Drive", "ArcadeDrive", "Command")) <<
			"Interrupted ArcadeDrive";
}

}



