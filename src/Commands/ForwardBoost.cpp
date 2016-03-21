/*
 * SpeedBoost.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#include "ForwardBoost.h"

#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

namespace commands
{

using namespace dman;

void ForwardBoost::Initialize()
{
	if(oi_ == nullptr)
	{
		Cancel();
		TextLog::Log(MessageData(MessageData::ERR), SystemData("OI", "ForwardBoost", "Command")) <<
				"Initialization failed";
		return;
	}

	oi_->SetForwardMultiplier(power_);
	TextLog::Log(MessageData(MessageData::INFO), SystemData("OI", "ForwardBoost", "Command")) <<
			"Initialized, setting power to: " << power_;
}

void ForwardBoost::End()
{
	if(oi_ == nullptr)
		return;

	oi_->SetForwardMultiplier(1.0);
	TextLog::Log(MessageData(MessageData::INFO), SystemData("OI", "ForwardBoost", "Command")) <<
			"Ending, reverting power to: " << 1.0;
}

void ForwardBoost::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("OI", "ForwardBoost", "Command")) <<
			"Interrupted";
	End();
}

}


