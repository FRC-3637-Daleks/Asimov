/*
 * WaitUntil.h
 *
 *  Created on: Apr 6, 2016
 *      Author: Edward
 */

#ifndef SRC_WAITUNTIL_H_
#define SRC_WAITUNTIL_H_

#include "WPILib.h"
#include "Utility/ValueStore.h"

#include <functional>

namespace commands
{


class WaitUntil: public Command
{
public:
	using Func_t = std::function<bool()>;

public:
	WaitUntil(Func_t func);
	WaitUntil(ValueStore::Value<bool> val);

public:
	void Initialize() override {};
	bool IsFinished() override;
	void Execute() override {};
	void Interrupted() override;

private:
	Func_t func_;
};




}



#endif /* SRC_WAITUNTIL_H_ */
