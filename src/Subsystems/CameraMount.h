/*
 * CameraMount.h
 *
 *  Created on: Feb 20, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_CAMERAMOUNT_H_
#define SRC_SUBSYSTEMS_CAMERAMOUNT_H_

// WPI Includes
#include "WPILib.h"

// STD Includes
#include <memory>

namespace subsystems
{

class CameraMount: public Subsystem
{
public:
	enum CamState_t: int16_t {DYNAMIC = -1, BACK = 0, GOAL, FORWARD, N_STATES};
	static double positions[N_STATES];

public:
	CameraMount();
	virtual ~CameraMount() = default;

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

public:
	bool is_initialized() const {return servo_ != nullptr;}
	bool doConfigure();

private:
	void initServo();

private:
	std::unique_ptr<Servo> servo_;
	CamState_t state_;
};

}

#endif /* SRC_SUBSYSTEMS_CAMERAMOUNT_H_ */
