#include <logger.hxx>

#include <iostream>

using namespace std;

ofstream Logger::logfile(LOGFILE);

Logger::Logger() {
	//TODO: Check logfile validity
}

Logger::~Logger() {
	cout << endl;
	logfile.close();
}

void Logger::log(string msg, Logger::Flush flush) {
	//TODO: Implement Flush options
	char s[32];
	time_t *t = new time_t;

	time(t);
	strftime(s, 32, "%x %X", localtime(t));
	cout << "[" << s << "] " << msg << endl;

	delete t;
}
