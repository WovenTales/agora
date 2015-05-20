#ifndef FEED_H
#define FEED_H

#include <iostream>
#include <pugixml.hpp>
#include <sqlite3.h>
#include <string>
#include <string.h>
#include <time.h>

enum FeedLang {
	RSS,
	ATOM
};

// Requires definition of FeedLang
#include "entry.hxx"

using namespace std;

class Feed {
  private:
	Feed();

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

	void initialize(pugi::xml_document&);
	void parseAtom(pugi::xml_node&);
	void parseRss(pugi::xml_node&);

  public:
	Feed(char *filename);

	//TODO: see http://atomenabled.org/developers/syndication/#text
	static const char *parseAtomTitle(const pugi::xml_node &t) { return t.child_value(); };

	void print();
};

#endif
