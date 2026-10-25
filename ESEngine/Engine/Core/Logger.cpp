#include "Logger.h"
#include <chrono>
#include <ctime>

const char* Logger::levelName[] = { "INFO", "WARN", "ERROR" };


void Logger::log(LogLevel level, std::string message) {
	std::cout << getCurrentTime() << " [" << levelName[level] << "][" << className << "] "<< message << std::endl;
}

std::string Logger::getCurrentTime() {
	using namespace std::chrono;
	system_clock::time_point now = system_clock::now();
	std::time_t seconds = system_clock::to_time_t(now);
	int micros = int(duration_cast<microseconds>(now - system_clock::from_time_t(seconds)).count());

	std::tm timeLocal;
#ifdef _WIN32
	localtime_s(&timeLocal, &seconds);
#else
	localtime_r(&seconds, &timeLocal);
#endif

	std::stringstream sstream;

	sstream << formatValue(timeLocal.tm_hour, 2) << ":" << formatValue(timeLocal.tm_min, 2) << ":" << formatValue(timeLocal.tm_sec, 2) << "." << formatValue(micros, 3);

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