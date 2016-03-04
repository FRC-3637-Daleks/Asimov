/*
 * FlipFront.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#include "FlipFront.h"

namespace commands
{

FlipFront::FlipFront(OI_t *oi, CameraMount_t *camera): oi_(oi), camera_(camera)
{
	Requires(camera_);
	SetRunWhenDisabled(true);
}

void FlipFront::Initialize()
{
	camera_->SetState(CameraMount_t::BACK);
	oi_->SetForward(false);
	oi_->Log(dman::MessageData::STATUS, "FlipFront", "Command") << "Initialize Complete. Controls now reversed";
}

void FlipFront::End()
{
	camera_->SetState(CameraMount_t::WHEEL);
	oi_->SetForward(true);
	oi_->Log(dman::MessageData::STATUS, "FlipFront", "Command") << "End Complete. Controls now normal";
}


}


