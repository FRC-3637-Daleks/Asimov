/*
 * XBoxWrapper.h
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_XBOXWRAPPER_H_
#define SRC_SUBSYSTEMS_XBOXWRAPPER_H_

#include "Utility/ValueStore.h"
#include "WPILib/WPISystem.h"

#include "WPILib.h"

#include <memory>

namespace subsystems
{

using namespace dman;

class XBoxWrapper: public WPISystem
{
public:
	using XBoxPtr_t = std::unique_ptr<Joystick>;

public:
	enum XBoxButton_t: uint8_t {A = 1, B, X, Y, L_TRIGGER, R_TRIGGER, BACK, START, L_STICK, R_STICK, N_BUTTONS = R_STICK};
	enum XBoxAxis_t: uint8_t {L_XAXIS = 0, L_YAXIS, L_TRIG, R_TRIG, R_XAXIS, R_YAXIS, N_AXES};

public:
	static std::string ButtonToString(XBoxButton_t button);
	static std::string AxisToString(XBoxAxis_t axis);

public:
	XBoxWrapper();
	virtual ~XBoxWrapper() = default;

public:
	bool is_initialized() {return xbox_ != nullptr;}
	void doRegister() override;
	bool doConfigure() override;

private:
	void initXBox();

private:
	XBoxPtr_t xbox_;
};

}





#endif /* SRC_SUBSYSTEMS_XBOXWRAPPER_H_ */
