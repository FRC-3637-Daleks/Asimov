/*
 * CameraMount.cpp
 *
 *  Created on: Feb 20, 2016
 *      Author: Edward
 */

#include "Camera.h"

#include "Utility/FunkyGet.h"
#include "Commands/SetCamera.h"

namespace subsystems
{

double Camera::positions[] = {
		/*[BACK] =*/ 0.0,
		/*[GOAL] =*/ 0.7,
		/*[WHEEL] =*/ 1.0
};

Camera::View Camera::views[] = {
		{0, true},	// BACK
		{0, false}, // GOAL
		{0, false}, // WHEEL
		{2, true},  // BALL
		{1, false}  // FRONT
};

Camera::Camera(): WPISystem("CameraMount"), servo_(nullptr), state_(DYNAMIC)
{
}

void Camera::SetState(CamState_t state)
{
	if(state < 0)
		return;

	state_ = state;

	if(state_ <= WHEEL)
	{
		SetPosition(positions[state_]);
	}

	if(state_ < N_STATES)
		PostView(views[state_]);
}

void Camera::PostView(const View &view)
{
	if(cam_table_)
	{
		cam_table_->PutNumber("stream_select", view.cam_id_);
		cam_table_->PutBoolean("flip", view.flip_);
	}
}

Camera::CamState_t Camera::GetState() const
{
	return state_;
}

void Camera::SetPosition(double position)
{
	if(is_initialized() && position >= -1.0 && position <= 1.0)
	{
		servo_->Set(position);
	}
}

double Camera::GetPosition() const
{
	if(is_initialized())
	{
		return servo_->Get();
	}

	return -1.0;
}

void Camera::OffsetPosition(double offset)
{
	SetPosition(GetPosition() + offset);
}

void Camera::doRegister()
{
	auto& ports = GetPortSpace("PWM");
	ports("servo");

	auto& settings = GetSettings();

	{
		auto& pos_settings = settings["positions"];
		pos_settings("BACK").SetDefault(positions[BACK]);
		pos_settings("GOAL").SetDefault(positions[GOAL]);
		pos_settings("WHEEL").SetDefault(positions[WHEEL]);
	}

	GetLocalValue<double>("position").Initialize(std::make_shared<FunkyGet<double> >([this](){
		return GetPosition();
	}));
}

bool Camera::doConfigure()
{
	if(!is_initialized())
		initServo();

	auto& settings = GetSettings();

	{
		auto& pos_settings = settings["positions"];
		positions[BACK] = pos_settings("BACK").GetValueOrDefault<double>();
		positions[GOAL] = pos_settings("GOAL").GetValueOrDefault<double>();
		positions[WHEEL] = pos_settings("WHEEL").GetValueOrDefault<double>();
		Log(MessageData(MessageData::INFO, 3), "", "") << "Cam Positions: {" <<
				"[BACK] = " << positions[BACK] << ", [GOAL] = " << positions[GOAL] << ", [WHEEL]"
				<< positions[WHEEL] << "}";
	}

	SetState(WHEEL);

	return true;
}

void Camera::initServo()
{
	if(!is_initialized())
	{
		auto& ports = GetPortSpace("PWM");
		servo_ = std::make_unique<Servo>(ports("servo"));
		cam_table_ = NetworkTable::GetTable("camera");
	}
}

commands::SetCamera *Camera::MakeSetCamera(CamState_t view)
{
	return new commands::SetCamera(this, view);
}

}
