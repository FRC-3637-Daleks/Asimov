/*
 * SetSwiss.cpp
 *
 *  Created on: Feb 19, 2016
 *      Author: elija_000
 */

#include <Commands/SetSwiss.h>
#include "SetSwiss.h"
#include "Commands/SetSwiss.h"
#include "Commands/Swiss.h"
#include <math.h>

SetSwiss::SetSwiss() {
	// TODO Auto-generated constructor stub
	 swiss_ = new CANTalon(x);

		// Default values:
		swing_speed_ = 0.5;

		state_ = State_t::OFF;
		mode_ = Mode_t::VBUS;
		SetMode(Mode_t::VELOCITY);

		max_velocity_ = 1.0;
		allowed_error_ = 100;


}





SetSwiss::~SetSwiss() {
	// TODO Auto-generated destructor stub
}




void Swiss::SetMode(mode_t mode)
{
	if (mode_ != mode)
	{
		mode_ = mode;
		if (mode_ == mode_t::VELOCITY)
			swiss_->SetControlMode(CANTalon::ControlMode::kSpeed);
		else if (mode_ == Mode_t::VBUS)
			swiss_->SetControlMode(CANTalon::ControlMode::kPercentVbus);
	}
}
