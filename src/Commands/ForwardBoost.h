/*
 * SpeedBoost.h
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_FORWARDBOOST_H_
#define SRC_COMMANDS_FORWARDBOOST_H_

#include "Subsystems/OI.h"

namespace commands
{

class ForwardBoost: public Command
{
public:
	using OI_t = subsystems::OI;

public:
	ForwardBoost(OI_t *oi, double boost_power): oi_(oi), power_(boost_power)
	{
		SetRunWhenDisabled(true);
	}

	virtual ~ForwardBoost() = default;

public:
	/// Sets the forward to power_
	void Initialize() override;

	/// Nada
	void Execute() override {}

	/// Always returns false
	bool IsFinished() override {return false;}

	/// Sets the mutlipliers back to what they were
	void End() override;

	/// Calls End
	void Interrupted() override;

private:
	OI_t *oi_;
	double power_;
};



}



#endif /* SRC_COMMANDS_FORWARDBOOST_H_ */
