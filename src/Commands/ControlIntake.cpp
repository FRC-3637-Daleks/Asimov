/*
 * ControlIntake.cpp
 *
 *  Created on: Apr 7, 2016
 *      Author: Edward
 */

#include "ControlIntake.h"

namespace commands
{

ControlIntake::ControlIntake(Intake_t intake, Input_t input):
		intake_(intake), input_(std::move(input))
{
	Requires(intake_);
}

void ControlIntake::Initialize()
{
	if(intake_ != nullptr /*&& intake_->is_initialized()*/ && input_.initialized())
	{
		intake_->Log(MessageData::STATUS, "ControlIntake", "Command") <<
				"Initialized with initial input = "  << input_.GetValue();
	}
	else
	{
		intake_->Log(MessageData::ERR, "ControlIntake", "Command") <<
					"Intake was not initialized. Canceling";
		Cancel();
	}
}

void ControlIntake::Execute()
{
	intake_->SetSpeed(input_.GetValue());
}

bool ControlIntake::IsFinished()
{
	return false;
}

void ControlIntake::End()
{
	if(intake_ != nullptr)
	{
		intake_->SetSpeed(0.0);
		intake_->Log(MessageData::STATUS, "ControlIntake", "Command") <<
			"Ended";
	}
}

void ControlIntake::Interrupted()
{
	if(intake_ != nullptr)
	{
		intake_->Log(MessageData::INFO, "ControlIntake", "Command") <<
			"Interrupted";
		End();
	}
}


}




