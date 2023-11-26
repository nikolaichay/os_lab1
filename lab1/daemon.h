#include <fstream>
#include <thread>
#include <unistd.h>
#include <csignal>
#include <chrono>
#include <string>
#include <syslog.h>

#ifndef DAEMON_H
#define DAEMON_H

class Daemon {
    Daemon(const Daemon&) = delete;
    Daemon(Daemon&&) = delete;
    Daemon& operator=(const Daemon&) = delete;
    Daemon& operator=(Daemon&&) = delete;
public:
    static Daemon& get_instance() {
        static Daemon instance = Daemon();
        return instance;
    }
    void set_cfg_path(const std::string& inp_cfg_path);
    void launch() const;
private:
    Daemon();
    void init() const;
    static void h_sighup(int sig);
    static void h_sigterm(int sig);
    static void read_cfg();
    void task() const;
    const std::string pid_path = "/var/run/daemon.pid";
    const std::string proc_path = "/proc";
    static std::string dir_path;
    static std::string cfg_path;
    const unsigned int time = 15;
};

#endif
