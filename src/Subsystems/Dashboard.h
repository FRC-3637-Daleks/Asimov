/*
 * Dashboard.h
 *
 *  Created on: Mar 25, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_DASHBOARD_H_
#define SRC_SUBSYSTEMS_DASHBOARD_H_

#include "WPILib.h"

#include "Utility/ValueStore.h"
#include "Utility/UpdateStore.h"
#include "Utility/Updateable.h"
#include "WPILib/WPISystem.h"

#include <string>
#include <vector>
#include <memory>

using namespace dman;

namespace subsystems
{

void SmartDashPublish(const std::string& key, double value);
void SmartDashPublish(const std::string& key, bool value);
void SmartDashPublish(const std::string& key, const std::string& value);


template<typename T>
class DashValue: public Updateable
{
public:
	using Value_t = ValueStore::Value<T>;
	using Key_t = std::string;

public:
	DashValue(Key_t key, Value_t value): publish_key_(key), value_(value)
	{
		// We must remove the freaking slashes from the publish_key because smart_dash is dumb
		for(unsigned int i = 0; i < publish_key_.size(); i++)
			if(publish_key_[i] == '/') publish_key_[i] = '_';
	}
	DashValue(Value_t value): DashValue(value.get_key(), std::move(value)) {}

	DashValue(const DashValue&) = default;
	DashValue(DashValue&&) = default;

	virtual ~DashValue() = default;

public:
	void doUpdate() override
	{
		if(value_.initialized())
			SmartDashPublish(publish_key_, value_.GetValue());
	}

private:
	Key_t publish_key_;
	Value_t value_;
};

class Dashboard: public WPISystem
{
public:
	Dashboard(const std::string& dash_name = "Dashboard");
	virtual ~Dashboard() = default;

public:
	void doRegister() override;
	bool doConfigure() override;

public:
	template<typename T> void AddDashValue(DashValue<T> dv)
	{
		dash_values_.push_back(std::make_unique<DashValue<T>>(std::move(dv)));
		GetUpdateStore(dash_name_).Add(dash_values_.back().get());
	}

	template<typename T> void AddDashValue(std::string name)
	{
		AddDashValue(DashValue<T>(GetValueStore().Get<T>(std::move(name))));
	}

	template<typename T> void AddDashValue(std::string publish_name, std::string value_name)
	{
		AddDashValue(DashValue<T>(publish_name, GetValueStore().Get<T>(std::move(value_name))));
	}

	UpdateThread& get_thread() {return GetUpdateThread(dash_name_);}

private:
	std::string dash_name_;
	std::vector<std::unique_ptr<Updateable>> dash_values_;
};

}



#endif /* SRC_SUBSYSTEMS_DASHBOARD_H_ */
