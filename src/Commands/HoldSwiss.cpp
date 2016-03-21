/*
 * HoldSwiss.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: Edward
 */

#include "HoldSwiss.h"

#include "Log/MessageData.h"

namespace commands
{

HoldSwiss::HoldSwiss(Swiss_t swiss): swiss_(swiss)
{
	Requires(swiss_);
}

void HoldSwiss::Initialize()
{
	if(swiss_ != nullptr && swiss_->is_initialized())
	{
		swiss_->Hold();
		swiss_->Log(MessageData::STATUS, "HoldSwiss", "Command") <<
				"Holding Swiss at " << swiss_->GetPos();
	}
	else
	{
		Cancel();
	}
}

}


