#include "my9221.h"

int main() {
  upm::MY9221* bar = new upm::MY9221(2, 3);
  	 
  while (true) {
    for (int i=1; i<=10; i++) {
      bar->setBarLevel(i, true);
      usleep(50000);
    }
    sleep(1);

    for (int i=1; i<=10; i++) {
      bar->setBarLevel(i, false);
  	usleep(50000);
    }
    sleep(1);
  }
}