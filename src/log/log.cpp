#include "log/log.hpp"

#include <iostream>
#include <fstream>
#include <exception>

#ifdef WIN32
#include <windows.h>
#include <direct.h>

#define mkdir(dir, mode) _mkdir(dir)
#else
#include <sys/stat.h>
#endif

using namespace stde::log;

std::string log::logfile() {
    mkdir("log", 0777);
    time_t rawtime;
    struct tm *info;
    char buffer[32];

    time(&rawtime);

    info = localtime(&rawtime);

    strftime(buffer, 32, "log/%Y%m%d%H%M%S.log", info);

    return std::string(buffer);
}

std::ofstream log::m_logfile;
stde::streams::tee_ostream log::m_outstream(std::cout, log::m_logfile);
bool log::m_init = false;
std::map<std::string, stde::log::log*> log::m_loggers;

stde::log::log& log::get(const std::string& name) {
    if (!m_init) {
        m_init = true;
        log::m_logfile = std::ofstream(logfile());
        // log::m_outstream = stde::streams::tee_ostream(std::cout, m_logfile);
        log l = get("logger");
        l << level::info << "Logger started on " << logtime() << "." << std::endl;
    }

    if (log::m_loggers.find(name) == log::m_loggers.end()) {
        log::m_loggers[name] = new log(name);
    }

    return *log::m_loggers[name];
}

void log::clean() {
    if (m_init) {
        for(auto it = log::m_loggers.begin(); it != log::m_loggers.end(); it++) {
            delete (*it).second;
        }

        log::m_loggers.clear();

        m_init = false;
    }
}

log::log(const std::string& name) : m_name(name) {

}

log::~log() {
}

std::ostream& log::operator<<(const level l) {
    m_outstream << "[" << log::logtime() << "][" << log::level_name(l) << "][" << m_name << "] ";
    return m_outstream;
}
