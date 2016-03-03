/*
 * SetSwiss.h
 *
 *  Created on: Feb 19, 2016
 *      Author: elija_000
 */

#ifndef SRC_COMMANDS_SETSWISS_H_
#define SRC_COMMANDS_SETSWISS_H_
#include "Subsystems/Swiss.h"

namespace commands
{

using Swiss = subsystems::Swiss;
class SetSwiss : public Command
{
	using state_t = Swiss::state_t;

public:
	SetSwiss(Swiss* swiss, state_t sta);
	virtual ~SetSwiss() = default;

public:

	void Initialize() override;


	void Execute() override ;


	bool IsFinished() override;


	void End() override;


	void Interrupted() override;


protected:

private:
	Swiss *swiss_;
	state_t stat;
};

} // end namespace commands


#endif /* SRC_COMMANDS_SETSWISS_H_ */
