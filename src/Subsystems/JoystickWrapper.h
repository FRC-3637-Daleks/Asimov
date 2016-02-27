/*
 * JoystickWrapper.h
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_JOYSTICKWRAPPER_H_
#define SRC_SUBSYSTEMS_JOYSTICKWRAPPER_H_

#include "Utility/ValueStore.h"
#include "WPILib/WPISystem.h"

#include "WPILib.h"

#include <memory>

namespace subsystems
{

using namespace dman;

class JoystickWrapper: public WPISystem
{
public:
	using JoystickPtr_t = std::unique_ptr<Joystick>;

public:
	JoystickWrapper(std::string hand_name);
	virtual ~JoystickWrapper() = default;

public:
	bool is_initialized() {return joy_ != nullptr;}
	void doRegister() override;
	bool doConfigure() override;

public:
	Joystick& get_joy() {return *joy_;}

private:
	void initJoystick();

private:
	JoystickPtr_t joy_;
};

}

#endif /* SRC_SUBSYSTEMS_JOYSTICKWRAPPER_H_ */
