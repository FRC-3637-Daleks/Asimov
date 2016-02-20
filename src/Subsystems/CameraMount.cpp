/*
 * CameraMount.cpp
 *
 *  Created on: Feb 20, 2016
 *      Author: Edward
 */

#include "CameraMount.h"

namespace subsystems
{

double CameraMount::positions[] = {
		[BACK] = 0.0,
		[GOAL] = 0.7,
		[FORWARD] = 1.0
};

CameraMount::CameraMount(): Subsystem("CameraMount"), servo_(nullptr), state_(BACK)
{
}

void CameraMount::SetState(CamState_t state)
{
	if(state >= 0 && state < N_STATES)
	{
		SetPosition(positions[state]);
		state_ = state;
	}
}

CameraMount::CamState_t CameraMount::GetState() const
{
	return state_;
}

void CameraMount::SetPosition(double position)
{
	if(is_initialized() && position > 0.0 && position < 1.0)
	{
		state_ = DYNAMIC;
		servo_->Set(position);
	}
}

double CameraMount::GetPosition() const
{
	if(is_initialized())
	{
		return servo_->Get();
	}

	return 0.0;
}

void CameraMount::OffsetPosition(double offset)
{
	SetPosition(GetPosition() + offset);
}

bool CameraMount::doConfigure()
{
	if(!is_initialized())
		initServo();

	return true;
}

void CameraMount::initServo()
{
	if(!is_initialized())
		servo_ = std::make_unique<Servo>(0);
}

}
