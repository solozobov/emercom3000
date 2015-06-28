#include <unistd.h>
#include "hmc5883l.h"

int main(int argc, char **argv)
{
	//! [Interesting]
	// Instantiate on I2C
	upm::Hmc5883l* compass = new upm::Hmc5883l(0);
	int16_t *pos;
	compass->set_declination(0.2749); // Set your declination from true north in radians
	// Print out the coordinates, heading, and direction every second
	while(true){
		compass->update(); // Update the coordinates
		pos = compass->coordinates();
		fprintf(stdout, "coor: %5d %5d %5d ", pos[0], pos[1], pos[2]);
		fprintf(stdout, "heading: %5.2f direction: %3.2f\n", compass->heading(), compass->direction());
		sleep(1);
	}
	//! [Interesting]
	return 0;
}
