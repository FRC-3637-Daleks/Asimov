/*
 * Grip.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: Edward
 */

#include "Grip.h"

#include "Utility/FunkyGet.h"

#include <math.h>

namespace subsystems
{

using namespace dman;

GRIP::GRIP(): WPISystem("GRIP"),
		target_center_x_(113.0), standard_offset_x_(50.0), allowed_error_x_(2.0), timeout_(1000), left_bias_(true),
		goals_(nullptr), center_x_(113.0)
{
}

double GRIP::GetOffsetX() const
{
	return GetCenterX() - get_target_center_x();
}

double GRIP::GetStandardErrorX() const
{
	return GetOffsetX() / get_standard_offset_x();
}

bool GRIP::IsCloseEnough() const
{
	return fabs(GetOffsetX()) < get_allowed_error_x();
}

void GRIP::doRegister()
{
	auto& goal_settings = GetSettings();
	auto& settings = goal_settings;

	goal_settings("target_center_x").SetDefault(get_target_center_x());
	goal_settings("standard_offset_x").SetDefault(get_standard_offset_x());
	goal_settings("allowed_error_x").SetDefault(get_allowed_error_x());

	settings("timeout").SetDefault(get_timeout());
	settings("timeout").SetDescription("milliseconds");

	goals_ = NetworkTable::GetTable("GRIP/goals");
	goals_->AddTableListener(this, true);

	GetLocalValue<double>("turn_output").Initialize(std::make_shared<FunkyGet<double>>([this] ()
			{
				return GetStandardErrorX();
			}));
	GetLocalValue<bool>("aligned").Initialize(std::make_shared<FunkyGet<bool>>([this] ()
			{
				return IsCloseEnough();
			}));
}

bool GRIP::doConfigure()
{
	Log(MessageData::STATUS, "", "") << "Configuring GRIP client";
	auto& goal_settings = GetSettings();
	auto& settings = goal_settings;

	set_target_center_x(goal_settings("target_center_x").GetValueOrDefault<double>());
	set_standard_offset_x(goal_settings("standard_offset_x").GetValueOrDefault<double>());
	set_allowed_error_x(goal_settings("allowed_error_x").GetValueOrDefault<double>());
	set_timeout(settings("timeout").GetValueOrDefault<int>());

	return true;
}

void GRIP::ValueChanged(ITable *source,
		llvm::StringRef key,
		std::shared_ptr<nt::Value> value,
		bool isNew)
{
	Log(MessageData(MessageData::INFO, 3), "", "") << "p l e a s e";
	if(value == nullptr)
		return;
	if(source == goals_.get())
	{
		if(value != nullptr && key == "centerX" && value->IsDoubleArray())
		{
			/*
			// TODO: std::async thread timeout code which nullifies our values if there's no update in time
			if(value->last_change() > get_timeout())
			{
				current_x_ =
				return;
			}
			*/

			auto array = value->GetDoubleArray();
			if(array.size() == 0)  // nothing found
			{
				// assume it was the previous value
			}
			else
			{
				double next_x = center_x_;

				if(array.size() == 1)
				{
					next_x = array[0];
				}
				else
				{
					double best = left_bias_? 100000:-100000;
					for(auto x : array)
					{
						if((left_bias_ && x < best) || (!left_bias_ && x > best))
							best = x;
					}

					next_x = best;
				}

				center_x_ = next_x;
			}


		}
	}

}


}


