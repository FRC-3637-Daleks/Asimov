/*
 * Intake.h
 *
 *  Created on: Jan 27, 2016
 *      Author: David
 */

#ifndef SRC_SUBSYSTEMS_INTAKE_H_
#define SRC_SUBSYSTEMS_INTAKE_H_
#include "WPILib.h"

class Intake : public Subsystem
{
public:
	Intake();

	virtual ~Intake();

public:
	/** Returns true if the mechanical switch is pressed on, and false otherwise.
	 * This is used to determine whether or not to stop the front rollers
	 */
	bool CheckSwitch() const;

	/** Controls the front rollers to take in and store the boulder. It will
	 * call the SetSpeed function to control the speed of the rollers and
	 * periodically call the CheckSwitch function to determine if the rollers
	 * should be stopped.
	 */
	void IntakeBall();

	/** Controls the front rollers in order to horizontally push out the boulder
	 * from its storage. It will call the SetSpeed function to control the speed
	 * of the rollers and periodically call the CheckSwitch function to determine
	 *  if the rollers should be stopped
	 */
	void PushBall();

	/** Sets the speed and direction of the front roller. The argument speed is a
	 * double between -1 and 1. 1 and -1 represent the roller's maximum speed in
	 * two different directions. Negative values for speed indicate that the roller
	 * should move outward (push), while positive values indicate a move inward (intake)
	 */
	void SetSpeed(double speed);

	/** Returns the current speed of the front roller (a value between -1 and 1)
	 */
	double GetSpeed() const;

protected:

private:

	CANTalon *roller_;
	DigitalInput *detector_;

};

#endif /* SRC_SUBSYSTEMS_INTAKE_H_ */
