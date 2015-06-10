#ifndef AGORA_H
#define AGORA_H

#include <pugixml.hpp>
#include <string>
#include <time.h>

//! Wrapper for global utility functions.
namespace agora {
	//! Names recognized feed languages.
	enum FeedLang {
		UNKNOWN_LANG,
		RSS,
		ATOM
	};

	//! Replace all instances of one substring with another.
	std::string replaceAll(std::string, const std::string&, const std::string&);

	//! Parse (potentially complex) Atom `<title>` node to string.
	std::string parseAtomTitle(const pugi::xml_node&);
	//! Parse variably-formatted string to standard time.
	time_t parseTime(std::string);
}

#endif
