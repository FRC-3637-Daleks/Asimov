/*
 * Shooter.h
 *
 *  Created on: Feb 1, 2016
 *      Author: David
 */

#ifndef SRC_SUBSYSTEMS_SHOOTER_H_
#define SRC_SUBSYSTEMS_SHOOTER_H_
#include "WPILib.h"

//spin up, spin down, spin one direction, variable speed
// remember commands namespace and make commands function

/**
 * Subsystems namespace for carrying
 * Shooter class definition over to
 * command files
 */
namespace subsystems
{

class Shooter : public Subsystem
{
// Constructor and Destructor:
public:
	Shooter();
	virtual ~Shooter();

// Functions:
public:
	/**
	 * Initializes settings of the top roller by calling CANTalon
	 * functions. The roller sets a Quadrature Encoder as its
	 * feedback device, sets maximum and minimum voltage output,
	 * and limits the roller to spin in only one (positive) direction.
	 */
	void Initialize();
	/**
	 * Controls the shooting roller by setting its speed and spinning it
	 * in an upward direction.
	 *
	 * @param speed the speed of the roller (double between -1.0 and 1.0)
	 * defaulted to 1.0
	 */
	void SpinUp(double speed = 1.0);

	/**
	 * Stops the shooting roller gradually by slowing it down.
	 */
	void SpinDown();

protected:

private:

	CANTalon *top_roller_;
};

}// end namespace subsystems
//

#endif /* SRC_SUBSYSTEMS_SHOOTER_H_ */
