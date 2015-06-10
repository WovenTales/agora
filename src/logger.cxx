#include <logger.hxx>

#include <iostream>
#include <sstream>

using namespace std;

Logger Logger::logger;
Logger::ofspacingstream Logger::logfile(LOGFILE, ofstream::app);

Logger::Logger() {
	//TODO: Check logfile validity
}

Logger::Flush operator|(Logger::Flush a, Logger::Flush b) {
	return (Logger::Flush)((int)a | (int)b);
}

Logger::ofspacingstream::~ofspacingstream() {
	if (!terminated) {
		*this << endl;
	}
	*this << endl;
}
