#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>
#include <string>


//! Utility class allowing simple logging to either file or standard output streams.
/*! \warning Does not recognize std::endl. Use Logger::ENDL instead.
 */
class Logger {
  private:
	enum CommandBit {
		// Use powers of two to allow bitwise OR
		ENDL_BIT     = (1 << 0),
		CONTINUE_BIT = (1 << 1),
		FORCE_BIT    = (1 << 2),
		ERROR_BIT    = (1 << 3)
	};
	std::ofstream *logfile;

	bool &error;
	bool &terminated;
	bool &continued;

	unsigned char &count;

	std::ostream &stream();

	Logger &operator--();
	Logger &operator++() { ++count; };

  public:
	//! Standard constructor.
	Logger(std::string = "");
	//! Copy constructor.
	Logger(const Logger&);
	//! Standard destructor.
	virtual ~Logger();

	//! Control commands handling manner to stream to log.
	/*! Can be combined with the standard OR operator `|`:
	 *  \code
	 *  Logger::ERROR | Logger::CONTINUE
	 *  \endcode
	 */
	enum Command {
		//! Don't insert a line break, flowing into next call.
		CONTINUE = CONTINUE_BIT,
		//! Print with error decorators, or to error stream.
		ERROR    = ERROR_BIT | ENDL_BIT,
		//! Insert a line break after message.
		ENDL     = ENDL_BIT,
		//! Force line break *before* the current mesage.
		FORCE    = FORCE_BIT | ENDL_BIT
	};

	//! Standard assignment operator.
	Logger &operator=(const Logger&);
	//! Insertion operator handling control commands (Logger::Command).
	Logger &operator<<(const Command&);

#include <logger.tcc>
};
//! Global handle to default logfile.
/*! Change destination by defining LOGFILE at compile; default of "" outputs to standard streams.
 */
extern Logger Log;

//! Standard bitwise OR.
inline Logger::Command operator|(const Logger::Command &l, const Logger::Command &r) { return (Logger::Command)((int)l | (int)r); };

#endif
