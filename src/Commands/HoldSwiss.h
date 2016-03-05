/*
 * HoldSwiss.h
 *
 *  Created on: Mar 3, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_HOLDSWISS_H_
#define SRC_COMMANDS_HOLDSWISS_H_

#include "WPILib.h"

#include "Subsystems/Swiss.h"

namespace commands
{

using namespace dman;

/** Calls Hold once on Swiss which will set the Swiss to hold its current position
 */
class HoldSwiss: public Command
{
public:
	using Swiss_t = subsystems::Swiss *;

public:
	HoldSwiss(Swiss_t swiss);
	virtual ~HoldSwiss() = default;

public:
	/// Holds swiss
	void Initialize() override;

	/// Nada
	void Execute() override {}

	/// Returns True
	bool IsFinished() override {return false;}

	/// Nada
	void End() override {}

	/// Nada
	void Interrupted() override {}

private:
	Swiss_t swiss_;
};


}



#endif /* SRC_COMMANDS_HOLDSWISS_H_ */
