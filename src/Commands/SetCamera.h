/*
 * SetCamera.h
 *
 *  Created on: Mar 4, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_SETCAMERA_H_
#define SRC_COMMANDS_SETCAMERA_H_

#include "WPILib.h"

#include "Subsystems/Camera.h"

namespace commands
{

class SetCamera: public Command
{
public:
	using Camera_t = subsystems::Camera;
	using CameraPtr_t = Camera_t *;

public:
	SetCamera(CameraPtr_t camera, Camera_t::CamState_t view);
	virtual ~SetCamera() = default;

public:
	/// Sets camera to the state passed in
	void Initialize() override;

	/// Nada
	void Execute() override {};

	/// Returns true
	bool IsFinished() override {return false;}

	/// Nada
	void End() override;

	/// Calls End
	void Interrupted() override {End();}

private:
	CameraPtr_t camera_;
	Camera_t::CamState_t view_;
};



}



#endif /* SRC_COMMANDS_SETCAMERA_H_ */
