/*
 * SetSwiss.cpp
 *
 *  Created on: Feb 19, 2016
 *      Author: elija_000
 */

#include "Subsystems/Swiss.h"
#include "Commands/SetSwiss.h"
#include <math.h>
namespace commands{

SetSwiss::SetSwiss(Swiss *swiss, state_t sta) {
	swiss_ = swiss;
	stat = sta;
	Requires(swiss_);
}


void SetSwiss::Initialize() {
	swiss_->SetState(stat);
}


void SetSwiss::Execute()  {
	swiss_->RefreshState();
}


bool SetSwiss::IsFinished() {
	return swiss_->IsCloseNuff();
}


void SetSwiss::End() {
	swiss_->RefreshState();
}


void SetSwiss::Interrupted() {
	End();
}


}
