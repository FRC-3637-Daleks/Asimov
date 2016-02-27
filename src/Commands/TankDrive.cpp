/*
 * TankDrive.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: Edward
 */

#include "TankDrive.h"
#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

namespace commands
{

using namespace dman;

TankDrive::TankDrive(Drive_t *drive, Input_t left_control, Input_t right_control, double multiplier, bool flip):
		drive_(drive),
		left_control_(std::move(left_control)),
		right_control_(std::move(right_control)),
		multiplier_(multiplier),
		flip_(flip)
{
	Requires(drive);
}

void TankDrive::Initialize()
{
	if(left_control_.initialized() && right_control_.initialized() && drive_ != nullptr)
	{
		TextLog::Log(MessageData(MessageData::INFO), SystemData("Drive", "TankDrive", "Command")) <<
				"Initializing TankDrive with initial left input: " << left_control_.GetValue() <<
				" and initial right input: " << right_control_.GetValue();
	}
	else
	{
		TextLog::Log(MessageData(MessageData::ERR), SystemData("Drive", "TankDrive", "Command")) <<
				"Failed to Initialize TankDrive";
		Cancel();
	}
}

void TankDrive::Execute()
{
	if(flip_)
		drive_->TankDrive(right_control_.GetValue() * multiplier_ * -1.0,
						  left_control_.GetValue() * multiplier_ * -1.0);
	else
		drive_->TankDrive(left_control_.GetValue() * multiplier_, right_control_.GetValue() * multiplier_);
}

void TankDrive::End()
{
	drive_->Stop();
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Drive", "TankDrive", "Command")) <<
			"Ending TankDrive";
}

void TankDrive::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("Drive", "TankDrive", "Command")) <<
			"Interrupted TankDrive";
}

}
