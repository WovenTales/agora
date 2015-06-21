// Contained in Logger class definition

//! Standard insertion operator.
/*! \warning Streams input directly to std::ostream; can only handle types
 *           recognized by normal eg. <tt>std::cout \<\<</tt> operator.
 */
template <typename T>
Logger &operator<<(const T &in) {
	if (terminated) {
		char s[32];
		time_t t;

		time(&t);
		strftime(s, 32, "%x %X", localtime(&t));

		std::string str("[");
		str.append(s);
		str.append("]");
		if (continued) {
			str.replace(0, std::string::npos, str.length(), ' ');
		}

		stream() << str << " ";
	}

	stream() << in;
	stream().flush();

	terminated = false;
	continued = false;

	return *this;
}
