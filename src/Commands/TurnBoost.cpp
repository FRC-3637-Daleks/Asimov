/*
 * TurnBoost.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#include "TurnBoost.h"

#include "Log/TextLog.h"
#include "Log/MessageData.h"
#include "Log/SystemData.h"

namespace commands
{

using namespace dman;

void TurnBoost::Initialize()
{
	if(oi_ == nullptr)
	{
		 Cancel();
		TextLog::Log(MessageData(MessageData::ERR), SystemData("OI", "TurnBoost", "Command")) <<
				"Initialization failed";
		return;
	}

	previous_power_ = oi_->get_turn_multiplier();
	oi_->SetTurnMultiplier(power_);
	TextLog::Log(MessageData(MessageData::INFO), SystemData("OI", "TurnBoost", "Command")) <<
			"Initialized, setting power to: " << power_;
}

void TurnBoost::End()
{
	if(oi_ == nullptr)
		return;

	oi_->SetTurnMultiplier(previous_power_);
	TextLog::Log(MessageData(MessageData::INFO), SystemData("OI", "TurnBoost", "Command")) <<
			"Ending, reverting power to: " << previous_power_;
}

void TurnBoost::Interrupted()
{
	TextLog::Log(MessageData(MessageData::INFO), SystemData("OI", "TurnBoost", "Command")) <<
			"Interrupted";
	End();
}

}





