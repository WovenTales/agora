#include <logger.hxx>

#include <iostream>

using namespace std;

Logger Logger::logger;
ofstream Logger::logfile(LOGFILE, ofstream::app);
bool Logger::terminated(true);

Logger::Logger() {
	//TODO: Check logfile validity
}

Logger::~Logger() {
	//TODO: Try to get this code called before we'd need to reopen logfile
	if (!logfile.is_open()) {
		logfile.open(LOGFILE, ofstream::app);
	}
	if (!terminated) {
		logfile << endl;
	}
	//TODO: Get separating blankline between runs working
	logfile << endl << endl;
	logfile.close();
}

Logger::Flush operator|(Logger::Flush a, Logger::Flush b) {
	return (Logger::Flush)((int)a | (int)b);
}

void Logger::log(string msg, Logger::Flush flush) {
	if ((flush & Logger::FORCE) && !terminated) {
		logfile << endl;
		terminated = true;
	}

	if (terminated) {
		char s[32];
		time_t t;

		time(&t);
		strftime(s, 32, "%x %X", localtime(&t));

		logfile << "[" << s << "] ";
	}

	logfile << msg;
	terminated = false;

	if (!(flush & Logger::CONTINUE)) {
		logfile << endl;
		terminated = true;
	}
}
