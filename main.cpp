#include "daemon.h"


int main(int argc, char *argv[]) {
  if(argc != 2) {
    exit(EXIT_FAILURE);
  }
  else {
    const Daemon &d = Daemon::get_instance(argv[1]);
    d.launch();
  }
  return 0; 
}
