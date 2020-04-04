#ifndef OWN_LOGGER_H
#define OWN_LOGGER_H

#include <string>
#include <iostream>
#include <cmath>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

enum LogLevel {
	LOG_INFO, LOG_WARN, LOG_ERROR
};

class Logger {
public:
	Logger(std::string className) : className(className) {};
	void log(LogLevel level, std::string info);

private:
	static const char* levelName[];
	std::string className;
	
	static std::string getCurrentTime();
	static std::string formatValue(int val, int space);
};

#endif