/*
 * Align.h
 *
 *  Created on: Feb 23, 2016
 *      Author: 3637-DS-1
 */
#include "WPILib.h"
#ifndef SRC_SUBSYSTEMS_ALIGN_H_
#define SRC_SUBSYSTEMS_ALIGN_H_

namespace subsystems{

class Align: public Subsystem
{
private:
	AnalogInput* Sensor1;
	AnalogInput* Sensor2;
	double Distance1;
	double Distance2;
	double TargetLeft; //constant
	double TargetRight; //constant


public:
	Align();
	double GetDistance1();
	double GetDistance2();
	void SetRightTarget(double tgt);
	void SetLeftTarget(double tgt);
	double LeftError();
	double RightError();
	double ForwardError();
	double RotationError();

};
}




#endif /* SRC_SUBSYSTEMS_ALIGN_H_ */
