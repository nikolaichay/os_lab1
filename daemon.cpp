#include "daemon.h"
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <sys/syslog.h>

namespace fs = std::filesystem;

std::string Daemon::cfg_path = "";
std::string Daemon::dir_path = "";


Daemon::Daemon(const std::string& inp_cfg_path) {
	openlog("daemon", LOG_PID, LOG_USER);
	if(!fs::exists(fs::absolute(inp_cfg_path).string())) {
		syslog(LOG_ERR, "config file doesn`t exist");
		closelog();
		exit(EXIT_FAILURE);
	}
	else {
		cfg_path = fs::absolute(inp_cfg_path).string();
		syslog(LOG_INFO, "config path read successfully");
		read_cfg(cfg_path);
	}
}

void Daemon::init() const {
	if (fs::exists(pid_path)) {
		std::ifstream pid_file(pid_path);
		pid_t existing_pid;
		if (pid_file >> existing_pid) {
			if (fs::exists(proc_path + "/" + std::to_string(existing_pid))) {
				syslog(LOG_WARNING, "another instance of daemon currently works");
				kill(existing_pid, SIGTERM);
				syslog(LOG_WARNING,"another instance of daemon killed successfully");
			}
		}
	}
	pid_t child = fork();
	if(child < 0) {
		syslog(LOG_ERR,"fork failed");
		closelog();
		exit(EXIT_FAILURE);
	}
	else if (child) {
		exit(EXIT_SUCCESS);
	}
	else {
		std::signal(SIGHUP, h_sighup);
		std::signal(SIGTERM, h_sigterm);

		std::ofstream pid_file(pid_path, std::ios::out);
		if (pid_file.is_open()) {
			pid_file << getpid();
		}
		else {
			syslog(LOG_ERR,"error while writing pid");
			exit(EXIT_FAILURE);
		}
	}
	syslog(LOG_INFO, "initialization successful");
}

void Daemon::launch() const {
	init();
	while (true) {
		task();
		std::this_thread::sleep_for(std::chrono::seconds(time));
	}
}

void Daemon::h_sighup(int sig) {
	syslog(LOG_INFO, "SIGHUP received, re-reading config files");
	read_cfg(cfg_path);
}

void Daemon::h_sigterm(int sig) {
	syslog(LOG_INFO, "SIGTERM received, terminating process...");
	closelog();
	exit(EXIT_SUCCESS);
}

void Daemon::task() const {
	if(fs::exists(dir_path) && fs::is_directory(dir_path) && !(fs::is_regular_file(dir_path + "/" + "dont.erase") && fs::exists(dir_path + "/" + "dont.erase"))) {
		for (const auto& entry : fs::recursive_directory_iterator(dir_path)) {
			if (fs::is_regular_file(entry)) {
				fs::remove(entry);
			}
		}
		for (const auto& subdir : fs::directory_iterator(dir_path)) {
			fs::remove_all(subdir);
		}	
	}
}

void Daemon::read_cfg(const std::string& cfg_path) {
	std::ifstream cfg_file(cfg_path);
	std::string rel_f = "";
	std::getline(cfg_file, rel_f);
	dir_path = fs::absolute(rel_f).string();

	if(!fs::exists(dir_path)) {
		syslog(LOG_ERR, "task: directory doesn`t exist");
		closelog();
		exit(EXIT_FAILURE);
	}
}
