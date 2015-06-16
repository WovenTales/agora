// Contained in namespace Logger

//! Log a message to the logfile.
/*! \p msgTemplate is passed into a std::ostringstream, and so this can handle any types that can.
 *
 *  Automatically prefixes date if message is printed at the beginning of the line.
 *
 *  \todo Handle multi-part messages instead of requiring multiple calls with Flush::CONTINUE
 *
 *  \param msgTemplate the message or object to log
 *  \param flush       the desired method of handling line endings
 */
template <typename T>
static void log(T msgTemplate, Flush flush = FLUSH) {
	std::ostringstream msg("");
	msg << msgTemplate;

	if ((flush & FORCE) && !logfile.terminated) {
		logfile << std::endl;
		logfile.terminated = true;
	}

	if (logfile.terminated) {
		char s[32];
		time_t t;

		time(&t);
		strftime(s, 32, "%x %X", localtime(&t));

		logfile << "[" << s << "] ";
	}

	logfile << msg.str();

	if (flush & CONTINUE) {
		logfile.flush();
		logfile.terminated = false;
	} else {
		logfile << std::endl;
		logfile.terminated = true;
	}
};
