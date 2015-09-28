#include <logger.hxx>

using namespace std;


#ifndef LOGFILE
#define LOGFILE ""
#endif
Logger Log(LOGFILE);


/*! Default \c filename of <tt>""</tt> outputs log entries to standard streams (std::cout and std::cerr).
 *
 *  \param filename the logfile to save to, or ""
 */
Logger::Logger(std::string filename) : count(*new unsigned char(1)), error(*new bool(false)), terminated(*new bool(true)), continued(*new bool(false)) {
	if (filename.empty()) {
		logfile = NULL;
	} else {
		//! \todo Handle bad logfile
		logfile = new ofstream(filename.c_str(), ofstream::app);
	}
}

/*! \param l the Logger to copy
 */
Logger::Logger(const Logger &l) : count(l.count), error(l.error), terminated(l.terminated), continued(l.continued) {
	logfile = l.logfile;

	++(*this);
}

Logger::~Logger() {
	--(*this);
}


std::ostream &Logger::stream() {
	if (logfile) {
		return *logfile;
	} else if (error) {
		return cerr;
	} else {
		return cout;
	}
}


Logger &Logger::operator=(const Logger &l) {
	--(*this);

	count      = l.count;
	error      = l.error;
	logfile    = l.logfile;
	terminated = l.terminated;
	continued  = l.continued;

	++(*this);
}

Logger &Logger::operator--() {
	if (--count == 0) {
		if (logfile) {
			*this << ENDL << FORCE;
			logfile->close();
			delete logfile;
		}

		delete &count;
		delete &error;
		delete &terminated;
		delete &continued;
	}
}

Logger &Logger::operator<<(const Logger::Command &in) {
	if (in & ENDL_BIT) {
		if ((in & FORCE_BIT) || (!terminated)) {
			stream() << endl;
			continued = false;
			terminated = true;
		}
		error = false;
	}

	// CONTINUE && same error state as current
	if ((in & CONTINUE_BIT) && ((in & ERROR_BIT) == error)) {
		continued = true;
	}

	if (in & ERROR_BIT) {
		error = true;
	}

	return *this;
}
