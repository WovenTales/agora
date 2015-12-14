#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>
#include <string>


//! Utility class allowing simple logging to either file or standard output streams
/*! \warning Does not properly recognize std::endl. Use Logger::ENDL instead.
 */
class Logger {
  private:
	//! Bitfield representation of commands
	enum CommandBit {
		// Use powers of two to allow bitwise OR
		ENDL_BIT     = (1 << 0),  //!< End line
		CONTINUE_BIT = (1 << 1),  //!< Don't print line decorators
		FORCE_BIT    = (1 << 2),  //!< Force a line break
		ERROR_BIT    = (1 << 3)   //!< Print as an error
	};

	//! Pointer to output file
	/*! If NULL, outputs to cout/cerr.
	 */
	std::ofstream *logfile;

	bool &error;           //!< Whether log is in an error state
	bool &terminated;      //!< Whether previous line has been terminated
	bool &continued;       //!< Whether log should continue previous message

	unsigned char &count;  //!< Keep references alive across leaving scope

	//! Currently active output
	std::ostream &stream();

	//! Decrement operator
	Logger &operator--();

  public:
	//! Standard constructor
	Logger(std::string = "");
	//! Copy constructor
	Logger(const Logger&);
	//! Standard destructor
	virtual ~Logger();

	//! Control commands handling manner to stream to log
	/*! Can be combined with the standard OR operator `|`:
	 *  \code
	 *  Logger::ENDL | Logger::CONTINUE
	 *  \endcode
	 */
	enum Command {
		/*! \warning Cleared on Logger::ENDL, so insert after or ORed with that command. */
		CONTINUE = CONTINUE_BIT,          //!< Print current line as part of last message
		ERROR    = ERROR_BIT | ENDL_BIT,  //!< Print with error decorators, or to error stream
		ENDL     = ENDL_BIT,              //!< Insert a line break and reset status
		FORCE    = FORCE_BIT | ENDL_BIT   //!< Force a line break, even if Logger::ENDL wouldn't have an effect
	};

	//! Standard assignment operator
	Logger &operator=(const Logger&);
	//! Insertion operator handling control commands (Logger::Command)
	Logger &operator<<(const Command&);

// public:
//   Logger &operator<<(const T&);
#include <logger.tcc>
};

//! Global handle to default logfile
/*! Change destination by defining LOGFILE at compile; default of "" outputs to standard streams.
 */
extern Logger Log;

//! Standard bitwise OR
inline Logger::Command operator|(const Logger::Command &l, const Logger::Command &r) { return (Logger::Command)((int)l | (int)r); };

#endif
