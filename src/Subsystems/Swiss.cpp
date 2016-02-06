#include "Swiss.h"
#include "WPILib.h"
#include <cmath>


Swiss::Swiss(int deviceNumber){
swisstalon = newCANTalon(deviceNumber);


}


int Swiss::getPosition(){


	return position;
}

void Swiss::maxheight(){
	SetMode(Mode_t::Position);

	//position = state[0];

}

void Swiss::minheight(){

	//position = state[3];

}


void Swiss::liftdoor(){

	//position = state[1];

}

void Swiss::lowercheval(){

	//position= state[2];

}
