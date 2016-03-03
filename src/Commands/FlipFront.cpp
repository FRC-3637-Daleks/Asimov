/*
 * FlipFront.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#include "FlipFront.h"

namespace commands
{

FlipFront::FlipFront(OI_t *oi, CameraMount_t *camera): oi_(oi), camera_(camera), table_(nullptr)
{
	table_ = NetworkTable::GetTable("camera");
	Requires(camera_);
	SetRunWhenDisabled(true);
}

void FlipFront::Initialize()
{
	camera_->SetState(CameraMount_t::BACK);
	oi_->SetForward(false);
	table_->PutNumber("stream_select", 0.0);
	table_->PutNumber("stream_angle", 180.0);
	oi_->Log(dman::MessageData::STATUS, "FlipFront", "Command") << "Initialize Complete. Controls now reversed";
}

void FlipFront::End()
{
	camera_->SetState(CameraMount_t::FORWARD);
	oi_->SetForward(true);
	table_->PutNumber("stream_select", 1.0);
	table_->PutNumber("stream_angle", 0.0);
	oi_->Log(dman::MessageData::STATUS, "FlipFront", "Command") << "End Complete. Controls now normal";
}


}


