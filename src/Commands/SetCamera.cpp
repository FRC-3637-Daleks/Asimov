/*
 * SetCamera.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Edward
 */

#include "SetCamera.h"
#include "Log/MessageData.h"

namespace commands
{

using namespace dman;

SetCamera::SetCamera(CameraPtr_t camera, Camera_t::CamState_t view):
		camera_(camera), view_(view)
{
}

void SetCamera::Initialize()
{
	if(camera_ != nullptr && camera_->is_initialized())
	{
		camera_->Log(MessageData::STATUS, "SetCamera", "Command") << "Initialized. Moving to state " << view_;
		camera_->SetState(view_);
	}
	else
	{
		camera_->Log(MessageData::ERR, "SetCamera", "Command") << "Camera wasn't initialized";
	}
}


}


