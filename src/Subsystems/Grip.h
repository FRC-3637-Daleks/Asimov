/*
 * Grip.h
 *
 *  Created on: Mar 29, 2016
 *      Author: Edward
 */

#ifndef SRC_SUBSYSTEMS_GRIP_H_
#define SRC_SUBSYSTEMS_GRIP_H_

#include "tables/ITableListener.h"
#include "tables/Itable.h"
#include "networktables/NetworkTable.h"

#include "WPILib/WPISystem.h"

#include <memory>

namespace subsystems
{

class GRIP: public dman::WPISystem, public ITableListener
{
public:
	GRIP();
	virtual ~GRIP() = default;

public:
	void set_target_center_x(double x) {target_center_x_ = x;}
	double get_target_center_x() const {return target_center_x_;}

	void set_standard_offset_x(double x) {standard_offset_x_ = x;}
	double get_standard_offset_x() const {return standard_offset_x_;}

	void set_allowed_error_x(double err) {allowed_error_x_ = err;}
	double get_allowed_error_x() const {return allowed_error_x_;}

	void set_left_bias(bool bias) {left_bias_ = bias;}
	bool get_left_bias() const {return left_bias_;}

	void set_timeout(int timeout) {timeout_ = timeout;}
	int get_timeout() const {return timeout_;}

	/// Returns the x position in pixels of the offset
	double GetCenterX() const {return center_x_;}

	/// Returns the difference between the target and current values
	double GetOffsetX() const;

	/// Returns a standard error from the target position in the camera
	double GetStandardErrorX() const;

	/// True if the robot is close enough
	bool IsCloseEnough() const;

public:
	/// Turns on the lights
	void ItsLit();

	/// Turns the lights off
	void BringItDown();

protected:
	void doRegister() override;
	bool doConfigure() override;

protected:
	void ValueChanged(ITable *source,
					llvm::StringRef key,
					std::shared_ptr<nt::Value> value,
					bool isNew) override;

private:
	double target_center_x_;
	double standard_offset_x_;
	double allowed_error_x_;
	int timeout_;
	bool left_bias_;	///< Refers to the expectation of the goal being the left most or right most contour

private:
	std::shared_ptr<ITable> goals_;
	std::unique_ptr<DigitalOutput> lights_;
	double center_x_;
};

}

#endif /* SRC_SUBSYSTEMS_GRIP_H_ */
