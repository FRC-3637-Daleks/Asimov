/*
 * ShootMode.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Edward
 */

#include "ShootMode.h"

#include "Log/MessageData.h"

namespace commands
{

ShootMode::ShootMode(OI_t oi, Camera_t camera, double forward_multiplier, double turn_multiplier):
		oi_(oi), camera_(camera), forward_multiplier_(forward_multiplier), turn_multiplier_(turn_multiplier)
{
	//Requires(camera_);
	Requires(oi_);
}

void ShootMode::Initialize()
{
	if(oi_ != nullptr || oi_->is_ready())
	{
		oi_->Log(dman::MessageData::STATUS, "ShootMode", "Command") << "Initiating shooting mode. " <<
						"forward_multiplier = " << forward_multiplier_ << ", turn_multiplier = " << turn_multiplier_;
		oi_->SetForwardMultiplier(forward_multiplier_);
		oi_->SetTurnMultiplier(turn_multiplier_);
	}
	else
	{
		oi_->Log(dman::MessageData::ERR, "ShootMode", "Command") << "OI not initialized";
	}

	if(camera_ != nullptr && camera_->is_ready())
	{
		//camera_->SetState(subsystems::Camera::GOAL);
	}
	else
	{
		camera_->Log(dman::MessageData::ERR, "ShootMode", "Command") << "Camera not initialized";
	}
}

void ShootMode::Execute()
{
}

bool ShootMode::IsFinished()
{
	return false;
}

void ShootMode::End()
{
	if(oi_ != nullptr && oi_->is_ready())
	{
		oi_->SetForwardMultiplier(1.0);
		oi_->SetTurnMultiplier(1.0);
		oi_->Log(dman::MessageData::STATUS, "ShootMode", "Command") << "Reverting to normal drive mode";
	}

	if(camera_ == nullptr && camera_->is_ready())
	{
		//camera_->SetState(subsystems::Camera::WHEEL);
		// Camera should revert to default state
	}
}

void ShootMode::Interrupted()
{
	if(oi_ != nullptr)
		oi_->Log(dman::MessageData::INFO, "ShootMode", "Command") << "Interrupted";
	End();
}

}
