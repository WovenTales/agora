#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

class Logger {
  private:
	static std::ofstream logfile;

  public:
	Logger();
	virtual ~Logger();

	enum Flush {
		CONTINUE,
		FLUSH,
		FORCE
	};

	static void log(const char *s, Flush b = FLUSH) { log(std::string(s), b); };
	static void log(std::string, Flush = FLUSH);
};

#endif
