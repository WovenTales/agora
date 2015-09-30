// Contained in namespace Logger

public:

//! Standard insertion operator
/*! \warning Streams input directly to std::ostream; can only handle types
 *           recognized by normal e.g. <tt>std::cout \<\<</tt> operator.
 *
 *  \param in the message to log
 */
template <typename T>
Logger &operator<<(const T &in) {
	// Print line decorators
	if (terminated) {
		char s[32];
		time_t t;

		time(&t);
		strftime(s, 32, "%x %X", localtime(&t));

		std::string str("[");
		str.append(s);
		str.append("]");

		if (continued) {
			// Blank out decoration
			str.replace(0, std::string::npos, str.length(), ' ');
		}

		// Need to use function as attempting to save resulting ostream throws error
		stream() << str << (error ? " ! " : "   ");
	}

	// Print message
	stream() << in;
	stream().flush();

	terminated = false;

	return *this;
}
