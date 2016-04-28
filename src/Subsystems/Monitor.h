/*
 * Monitor.h
 *
 *  Created on: Apr 16, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_MONITOR_H_
#define SRC_SUBSYSTEMS_MONITOR_H_

#include "WPILib/WPISystem.h"
#include "Utility/ValueStore.h"
#include "WPILib.h"

#include <functional>

namespace subsystems
{

/** Accepts a callback function, a fail function, and a fail time
 * In separate thread, if fail function fires for the length of the passed in time,
 * cancel the current command of the subsystem
 */
class MonitorEntry: public dman::Updateable
{
public:
	using Trigger_t = std::function<bool()>;
	using Callback_t = std::function<void()>;
	using Seconds_t = double;

public:
	MonitorEntry(Trigger_t trigger, Callback_t callback, Seconds_t timeout = 0.0);
	virtual ~MonitorEntry() = default;

public:
	/// Refreshes the timer depending on the state of the trigger and calls back if the timeout is expired
	void doUpdate() override;

public:
	Trigger_t trigger_;
	Callback_t callback_;
	Seconds_t timeout_;
	Timer timer;
};

class Monitor: public dman::WPISystem
{

};

}



#endif /* SRC_SUBSYSTEMS_MONITOR_H_ */
