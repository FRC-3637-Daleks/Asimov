/*
 * FlipFront.h
 *
 *  Created on: Feb 27, 2016
 *      Author: Edward
 */

#ifndef SRC_COMMANDS_FLIPFRONT_H_
#define SRC_COMMANDS_FLIPFRONT_H_

#include "WPILib.h"
#include "networktables/NetworkTable.h"

#include "Subsystems/OI.h"
#include "Subsystems/CameraMount.h"

#include <memory>

namespace commands
{

/** While this command is running, the robot's forward direction is behind itself
 * The camera will also be set to look behind itself
 */
class FlipFront: public Command
{
public:
	using OI_t = subsystems::OI;
	using CameraMount_t = subsystems::CameraMount;
	using ITable_t = std::shared_ptr<ITable>;

public:
	/** Constructs FlipFront command
	 * @param forward if true, sets the forward direction to the front of the robot
	 */
	FlipFront(OI_t *oi, CameraMount_t *camera);
	virtual ~FlipFront() = default;

public:
	/// Flips the OI and camera
	void Initialize() override;

	/// Nada
	void Execute() override {};

	/// Returns false
	bool IsFinished() override {return false;}

	/// Returns camera to face front
	void End() override;

	/// Calls End
	void Interrupted() override {End();}

private:
	OI_t *oi_;
	CameraMount_t *camera_;
	ITable_t table_;
};





}



#endif /* SRC_COMMANDS_FLIPFRONT_H_ */
