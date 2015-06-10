#include <logger.hxx>

#include <iostream>

using namespace std;

Logger Logger::logger;
Logger::ofspacingstream Logger::logfile(LOGFILE, ofstream::app);

Logger::Logger() {
	//! \todo Check logfile validity
}



Logger::Flush operator|(const Logger::Flush &a, const Logger::Flush &b) {
	return (Logger::Flush)((int)a | (int)b);
}



Logger::ofspacingstream::~ofspacingstream() {
	if (!terminated) {
		*this << endl;
	}
	*this << endl;
}
