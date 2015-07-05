#include <mraa.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <mraa/uart.h>
#include "led_bar.h"
#include "buzzer.h"
#include "hmc5883l.h"

#include <stdio.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

int minValue = 0;
int maxValue = 2300;

void updateMinMax(int value) {
    if (value < minValue) {
         minValue = value;
	}
	if (value > maxValue) {
		//maxValue = value;
	}
}

int main() {
	int INIT_SECONDS = 3;

	mraa_init();
	mraa_aio_context pir = mraa_aio_init(0);
	mraa_aio_set_bit(pir, 12);
	upm::MY9221* bar = new upm::MY9221(6, 7);
	upm::Buzzer* buz = new upm::Buzzer(3);
	buz->setVolume(0.9);
	buz->stopSound();
	int chord[] = {DO, DO, RE, MI, FA, SOL, LA, SI, DO, SI};

	mraa_uart_context m_uart = mraa_uart_init(0);
	char *devPath = mraa_uart_get_dev_path(m_uart);
	int fd = open(devPath, O_RDWR);	
	struct termios termio;
	tcgetattr(fd, &termio);
	cfmakeraw(&termio);
	cfsetispeed(&termio, B9600);
	cfsetospeed(&termio, B9600);2
	tcsetattr(fd, TCSAFLUSH, &termio);

	upm::Hmc5883l* compass = new upm::Hmc5883l(0);
	compass->set_declination(0.2749);

	sleep(3);
	unsigned int baseValue = 0;
	int counts = 0;
	clock_t start = clock();
	do {
		int value = mraa_aio_read(pir);
		updateMinMax(value);
		baseValue += value;
		counts++;
	} while (clock() - start < INIT_SECONDS * CLOCKS_PER_SEC);
	baseValue /= counts;

	int direction;
	clock_t last = clock();
	for (;;) {
		int value = mraa_aio_read(pir);
		updateMinMax(value);
        
        int direction;
        clock_t current = clock();
        if ((current - last) * 2 > CLOCKS_PER_SEC) {
			compass->update();
			direction = (int) compass->heading();
	    	dprintf(fd, "C %3d\n", direction);
            printf("%d\n", direction);
	    	last = current;
        }

		if (value < baseValue) {
	    	buz->stopSound();
	    } else {
	    	int dValue = (value - baseValue) * 11;
			int level = dValue  / (maxValue - baseValue);
			//printf("%d %d %d %d %d\n", baseValue, minValue, value, maxValue, level);
			if (level > 10) {
				level = 10;
			}

		    if (level > 5) {
		    	dprintf(fd, "D %3d\n", direction);
	        	buz->playSound(chord[level], 10000);
		    } else {
	        	buz->stopSound();
		    }

			bar->setBarLevel(level);
	    }
	}
}
