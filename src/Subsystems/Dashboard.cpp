/*
 * Dashboard.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: Edward
 */

#include "Dashboard.h"

namespace subsystems
{

Dashboard::Dashboard(const std::string &dash_name):
			WPISystem(dash_name),
			dash_name_(dash_name)
{
}

void Dashboard::doRegister()
{
	// Sets thread to update the update store. Passed in with null deleter
	get_thread().SetUpdater(std::shared_ptr<UpdateStore>(&GetUpdateStore(dash_name_), [](UpdateStore *){}));
}

bool Dashboard::doConfigure()
{
	if(!get_thread().is_running())
		get_thread().Start();
	return true;
}

void SmartDashPublish(const std::string& key, double value)
{
	SmartDashboard::PutNumber(key, value);
}

void SmartDashPublish(const std::string& key, bool value)
{
	SmartDashboard::PutBoolean(key, value);
}

void SmartDashboard(const std::string& key, const std::string& value)
{
	SmartDashboard::PutString(key, value);
}

}


