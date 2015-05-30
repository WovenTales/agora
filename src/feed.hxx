#ifndef FEED_H
#define FEED_H

#include "article.hxx"
#include "database.hxx"

#include <iostream>
#include <pugixml.hpp>
#include <sstream>
#include <string>
#include <string.h>
#include <time.h>

using namespace std;

class Feed {
  private:
	Feed();

	pugi::xml_node root;

	string id;
	string title;
	//TODO: Include in parsing functions (requires recognizing format of string)
	// Actually, only has limited utility (we've already downloaded the file)
	time_t updated;

	string author;
	//TODO: Contributor tags
	string link;
	string description;

	FeedLang lang;

	//TODO: Category tags (default/additional for entries)
	//TODO: Language

	void initialize(const pugi::xml_document&);
	void parseAtom(const pugi::xml_node&);
	void parseRss(const pugi::xml_node&);

  public:
	Feed(const char *filename);

	const void save(Database&);

	const void print();
};

#endif
