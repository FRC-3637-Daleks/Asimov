/*
 * align.h
 *
 *  Created on: Feb 22, 2016
 *      Author: elija_000
 */

#ifndef SRC_SUBSYSTEMS_ALIGN_H_
#define SRC_SUBSYSTEMS_ALIGN_H_

#include "WPILib.h"

namespace subsystems{

class Align:public Subsystem
{
public:
AnalogInput* Sensor1;
AnalogInput* Sensor2;

public:
	Align();

};
}

#endif /* SRC_SUBSYSTEMS_ALIGN_H_ */
