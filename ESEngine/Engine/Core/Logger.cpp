#include "Logger.h"

const char* Logger::levelName[] = { "INFO", "WARN", "ERROR" };


void Logger::log(LogLevel level, std::string message) {
	std::cout << getCurrentTime() << " [" << levelName[level] << "][" << className << "] "<< message << std::endl;
}

std::string Logger::getCurrentTime() {
	boost::posix_time::ptime timeLocal =
		boost::posix_time::microsec_clock::local_time();
	
	std::stringstream sstream;

	sstream << formatValue(timeLocal.time_of_day().hours(), 2) << ":" << formatValue(timeLocal.time_of_day().minutes(), 2) << ":" << formatValue(timeLocal.time_of_day().seconds(), 2) << "." << formatValue(timeLocal.time_of_day().fractional_seconds(), 3);

	return sstream.str();
}

std::string Logger::formatValue(int value, int space) {
	std::stringstream sstream;
	int places = pow(10, space-1);
	while (places > value) {
		places /= 10;
		sstream << "0";
	}
	
	if (value != 0)
		sstream << value;

	return sstream.str().substr(0, space);
}