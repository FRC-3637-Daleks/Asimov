/*
 * VarSpinUp.h
 *
 *  Created on: Apr 20, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_VARSPINUP_H_
#define SRC_COMMANDS_VARSPINUP_H_

#include "SpinUp.h"
#include "Utility/ValueStore.h"


namespace commands
{

/** A SpinUp which accepts a runtime speed modifier which is added to the base speed at initialization
 * This runtime speed modifier can be something like a joystick
 */
class VarSpinUp: public SpinUp
{
public:
	using Input_t = dman::ValueStore::Value<double>;

public:
	VarSpinUp(Shooter* shooter, Input_t modifier, double speed = 1.0, double time = 2.0);
	virtual ~VarSpinUp() = default;

public:
	/// Returns the base speed with the modifier's value taken away
	double GetSpeed() const override;

private:
	Input_t modifier_;
};


}



#endif /* SRC_COMMANDS_VARSPINUP_H_ */
