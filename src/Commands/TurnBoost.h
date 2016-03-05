/*
 * SpeedBoost.h
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_TURNBOOST_H_
#define SRC_COMMANDS_TURNBOOST_H_

#include "Subsystems/OI.h"

namespace commands
{

class TurnBoost: public Command
{
public:
	using OI_t = subsystems::OI;

public:
	TurnBoost(OI_t *oi, double boost_power): oi_(oi), power_(boost_power)
	{
		SetRunWhenDisabled(true);
	}
	virtual ~TurnBoost() = default;

public:
	/// Sets the turn to power_
	void Initialize() override;

	/// Nada
	void Execute() override {}

	/// Always returns false
	bool IsFinished() override {return false;}

	/// Sets the multipliers back to what they were
	void End() override;

	/// Calls End
	void Interrupted() override;

private:
	OI_t *oi_;
	double power_;
};

}


#endif /* SRC_COMMANDS_TURNBOOST_H_ */
