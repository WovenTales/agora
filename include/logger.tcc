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

		stream() << "[" << s << "] ";
	}

	stream() << in;
	stream().flush();

	terminated = false;

	return *this;
}
