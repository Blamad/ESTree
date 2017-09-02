#ifndef OWN_LOGGER_H
#define OWN_LOGGER_H

#include <string>
#include <iostream>
#include <cmath>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

using namespace std;

enum LogLevel {
	INFO, WARN, ERROR
};

class Logger {
public:
	Logger(string className) : className(className) {};
	void log(LogLevel level, string info);

private:
	static const char* levelName[];
	string className;
	
	static string getCurrentTime();
	static string formatValue(int val, int space);
};

#endif