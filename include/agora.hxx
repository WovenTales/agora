#ifndef AGORA_H
#define AGORA_H

#include <pugixml.hpp>
#include <string>
#include <time.h>
#include <vector>


//! Wrapper for global utility functions
namespace agora {
	//! Names recognized feed languages
	enum FeedLang {
		UNKNOWN_LANG,  //!< Unsupported feed format
		RSS,           //!< RSS 2.0
		ATOM           //!< ATOM
	};

	//! Download a file using cURL
	std::string download(const std::string&);

	//! Replace all instances of one substring with another
	std::string replaceAll(std::string, const std::string&, const std::string&);

	//! Parse (potentially complex) Atom `<title>` node to string
	std::string parseAtomTitle(const pugi::xml_node&);
	//! Parse variably-formatted string to standard time
	time_t parseTime(const std::string&);

	//! Split a string into a vector using the given delimiter
	std::vector<std::string> splitString(const std::string&, char = '\n');
	//! Split a collection of strings using the given delimiter
	std::vector<std::string> splitString(const std::vector<std::string>&, char = '\n');
}

#endif
