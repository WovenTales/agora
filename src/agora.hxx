#ifndef AGORA_H
#define AGORA_H

#include <string>

enum FeedLang {
	RSS,
	ATOM
};

std::string replaceAll(std::string, const std::string&, const std::string&);

#endif
