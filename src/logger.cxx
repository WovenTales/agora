#include <logger.hxx>

#include <iostream>

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
	

void Logger::log(string msg, Logger::Flush flush) {
	if ((flush & Logger::FORCE) && !logfile.terminated) {
		logfile << endl;
		logfile.terminated = true;
	}

	if (logfile.terminated) {
		char s[32];
		time_t t;

		time(&t);
		strftime(s, 32, "%x %X", localtime(&t));

		logfile << "[" << s << "] ";
	}

	logfile << msg;
	logfile.terminated = false;

	if (!(flush & Logger::CONTINUE)) {
		logfile << endl;
		logfile.terminated = true;
	}
}
