#include "Logger.h"

const char* Logger::levelName[] = { "INFO", "WARN", "ERROR" };


void Logger::log(LogLevel level, string message) {
	cout << getCurrentTime() << " [" << levelName[level] << "][" << className << "] "<< message << endl;
}

string Logger::getCurrentTime() {
	boost::posix_time::ptime timeLocal =
		boost::posix_time::second_clock::local_time();
	
	stringstream sstream;

	sstream << formatValue(timeLocal.time_of_day().hours(), 2) << ":" << formatValue(timeLocal.time_of_day().minutes(), 2) << ":" << formatValue(timeLocal.time_of_day().seconds(), 2);
	//sstream << "." << formatValue(timeLocal.time_of_day().total_milliseconds(), 4);

	return sstream.str();
}

string Logger::formatValue(int value, int space) {
	stringstream sstream;
	int places = pow(10, space-1);
	while (places > value) {
		places /= 10;
		sstream << "0";
	}
	
	if (value != 0)
		sstream << value;

	return sstream.str();
}