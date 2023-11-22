#include "daemon.h"


int main(int argc, char *argv[]) {
  if(argc != 2) {
    exit(EXIT_FAILURE);
  }
  else {
    Daemon &d = Daemon::get_instance();
    d.set_cfg_path(argv[1]);
    d.launch();
  }
  return 0; 
}
