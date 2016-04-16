/*
 * Monitor.h
 *
 *  Created on: Apr 16, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_MONITOR_H_
#define SRC_SUBSYSTEMS_MONITOR_H_

#include "WPILib/WPISystem.h"
#include "WPILib.h"

namespace subsystems
{

/** Accepts a subsystem with a fail function and fail time
 * In separate thread, if fail function fires for the length of the passed in time,
 * cancel the current command of the subsystem
 */
class Monitor: public dman::WPISystem
{

};

}



#endif /* SRC_SUBSYSTEMS_MONITOR_H_ */
