/*
 * VarSpinUp.cpp
 *
 *  Created on: Apr 20, 2016
 *      Author: Edward
 */

#include "VarSpinUp.h"

namespace commands
{

VarSpinUp::VarSpinUp(Shooter* shooter, Input_t modifier, double speed, double time):
		SpinUp(shooter, speed, time), modifier_(std::move(modifier))
{
}

double VarSpinUp::GetSpeed() const
{
	return SpinUp::GetSpeed() + modifier_.GetValueOr(0.0);
}


}


