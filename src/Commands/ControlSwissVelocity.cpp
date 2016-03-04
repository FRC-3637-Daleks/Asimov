/*
 * ControlSwissVelocity.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: Edward
 */

#include "ControlSwissVelocity.h"

namespace commands
{

ControlSwissVelocity::ControlSwissVelocity(Swiss_t swiss, Input_t input):
		swiss_(swiss), input_(std::move(input))
{
}

void ControlSwissVelocity::Initialize()
{
	if(swiss_ != nullptr && swiss_->is_initialized() && input_.initialized())
	{
		swiss_->SetMode(subsystems::Swiss::mode_t::vbus);
		swiss_->Log(MessageData::STATUS, "ControlSwissVelocity", "Command") <<
				"Initialized with initial input = "  << input_.GetValue();
	}
	else
	{
		swiss_->Log(MessageData::ERR, "ControlSwissVelocity", "Command") <<
					"Swiss was not initialized. Canceling";
		Cancel();
	}
}

void ControlSwissVelocity::Execute()
{
	swiss_->SetVelocity(input_.GetValueOr(0.0));
}

bool ControlSwissVelocity::IsFinished()
{
	return false;
}

void ControlSwissVelocity::End()
{
	if(swiss_ != nullptr)
	{
		swiss_->Hold();
		swiss_->Log(MessageData::STATUS, "ControlSwissVelocity", "Command") <<
			"Ended at " << swiss_->GetPos();
	}
}

void ControlSwissVelocity::Interrupted()
{
	if(swiss_ != nullptr)
	{
		swiss_->Log(MessageData::INFO, "ControlSwissVelocity", "Command") <<
			"Interrupted";
		End();
	}
}


}


