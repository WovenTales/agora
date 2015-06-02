#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

//TODO: Allow native inclusion of eg. int elements (currently requires external ostringstream)
class Logger {
  private:
	// Require calls of constructor/destructor
	static Logger logger;

	static std::ofstream logfile;

	static bool terminated;

  public:
	Logger();
	virtual ~Logger();

	enum Flush {
		// Use powers of two to allow bitwise OR
		FLUSH = 0,
		CONTINUE = 1,
		FORCE = 2
	};

	static void log(const char *s, Flush b = FLUSH) { log(std::string(s), b); };
	static void log(std::string, Flush = FLUSH);
};

Logger::Flush operator|(Logger::Flush a, Logger::Flush b);

#endif
