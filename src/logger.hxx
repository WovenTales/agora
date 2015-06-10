#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <sstream>
#include <string>

class Logger {
  private:
	// Require calls of constructor/destructor rather than never initializing
	static Logger logger;

	class ofspacingstream : public std::ofstream {
	  private:
		friend class Logger;

		bool terminated;

	  public:
		ofspacingstream(const char* filename,
		                ios_base::openmode mode = ios_base::out
		               ) : std::ofstream(filename, mode) {
			terminated = true; };
		virtual ~ofspacingstream();
	};
	static ofspacingstream logfile;

  public:
	Logger();

	enum Flush {
		// Use powers of two to allow bitwise OR
		FLUSH = 0,
		CONTINUE = 1,
		FORCE = 2
	};

#include <logger.tcc>
};

Logger::Flush operator|(Logger::Flush a, Logger::Flush b);

#endif
