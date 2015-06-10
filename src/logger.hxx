#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <sstream>
#include <string>

//! Singleton allowing logging of messages to a file
class Logger {
  private:
	// Force calls of constructor/destructor rather than never initializing
	static Logger logger;

	//! Wrapper for ofstream to insert extra blank line when closing file
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
  	//! Default constructor
	Logger();

	//! Allowed options for line endings
	/*! Can be combined with the standard OR operator |
	 *  \code
	 *  Logger::CONTINUE | Logger::FORCE
	 *  \endcode
	 */
	enum Flush {
		// Use powers of two to allow bitwise OR
		//! Insert a line break after message
		FLUSH = 0,
		//! Don't insert a line break, flowing into next call
		CONTINUE = 1,
		//! Force line break *before* the current mesage
		FORCE = 2
	};

#include <logger.tcc>
};

Logger::Flush operator|(Logger::Flush a, Logger::Flush b);

#endif
