/*
 * Align.h
 *
 *  Created on: Feb 23, 2016
 *      Author: 3637-DS-1
 */
#ifndef SRC_SUBSYSTEMS_ALIGN_H_
#define SRC_SUBSYSTEMS_ALIGN_H_

#include "WPILib.h"

#include "WPILib/WPISystem.h"

namespace subsystems{

using namespace dman;

class Align: public WPISystem
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
	double GetDistance1() const;
	double GetDistance2() const;
	void SetRightTarget(double tgt);
	void SetLeftTarget(double tgt);
	double LeftError() const;
	double RightError() const;
	double ForwardError() const;
	double RotationError() const;
	double GetLeftOutput() const;
	double GetRightOutput() const;
	double GetForwardOutput() const;
	double GetTurnOutput() const;

public:
	bool is_initialized() const {return Sensor1 != nullptr && Sensor2 != nullptr;}
	void doRegister() override;
	bool doConfigure() override;

private:
	void initSensors();

};
}

#endif /* SRC_SUBSYSTEMS_ALIGN_H_ */
