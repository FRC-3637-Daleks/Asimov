/*
 * ShootMode.h
 *
 *  Created on: Mar 4, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_SHOOTMODE_H_
#define SRC_COMMANDS_SHOOTMODE_H_

#include "WPILib.h"

#include "Subsystems/OI.h"
#include "Subsystems/Camera.h"

namespace commands
{

class ShootMode: public Command
{
public:
	using OI_t = subsystems::OI *;
	using Camera_t = subsystems::Camera *;

public:
	ShootMode(OI_t oi, Camera_t camera, double forward_multiplier = .2, double turn_multiplier = .2);
	virtual ~ShootMode() = default;

public:
	/// Sets camera to goal and drive speed to low
	void Initialize() override;

	/// Nada
	void Execute() override;

	/// Returns false
	bool IsFinished() override;

	/// Reverts settings
	void End() override;

	/// Calls End
	void Interrupted() override;

private:
	OI_t oi_;
	Camera_t camera_;
	double forward_multiplier_, turn_multiplier_;
};


}

#endif /* SRC_COMMANDS_SHOOTMODE_H_ */
