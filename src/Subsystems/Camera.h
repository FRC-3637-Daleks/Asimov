/*
 * CameraMount.h
 *
 *  Created on: Feb 20, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_CAMERA_H_
#define SRC_SUBSYSTEMS_CAMERA_H_

// WPI Includes
#include "WPILib.h"
#include "tables/ITable.h"
#include "networktables/NetworkTable.h"

// DMAN Inclueds
#include "WPILib/WPISystem.h"

// STD Includes
#include <memory>

namespace subsystems
{

using namespace dman;

class Camera: public WPISystem
{
public:
	using Table_t = std::shared_ptr<ITable>;

public:
	enum CamState_t: int16_t {DYNAMIC = -1, BACK = 0, GOAL, WHEEL, BALL, FRONT, N_STATES};
	static double positions[WHEEL+1];

public:
	struct View
	{
		int cam_id_;
		bool flip_;
		View(int cam_id, bool flip): cam_id_(cam_id), flip_(flip) {}
	};

	static View views[N_STATES];

public:
	Camera();
	virtual ~Camera() = default;

public:
	/// Sets the position of the servo to \c positions[state]
	void SetState(CamState_t state);

	/// Returns the current state of the servo
	CamState_t GetState() const;

	/// Directly sets the position of the servo
	void SetPosition(double position);

	/// Returns the current set point of the servo
	double GetPosition() const;

	/// Changes the position by offset
	void OffsetPosition(double offset);

	/// Posts to the smart dashboard which camera and angle to use
	void PostView(const View &v);

public:
	bool is_initialized() const {return servo_ != nullptr;}
	bool doConfigure();
	void doRegister();

private:
	void initServo();

private:
	std::unique_ptr<Servo> servo_;
	CamState_t state_;
	Table_t cam_table_;
};

}

#endif /* SRC_SUBSYSTEMS_CAMERA_H_ */
