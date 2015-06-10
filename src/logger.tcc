// Contained in namespace Logger

//TODO: Handle multi-part messages instead of requiring multiple calls with Flush::CONTINUE
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
	logfile.terminated = false;

	if (!(flush & CONTINUE)) {
		logfile << std::endl;
		logfile.terminated = true;
	}
};
