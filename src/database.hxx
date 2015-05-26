#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <pugixml.hpp>
#include <sqlite3.h>
#include <string>

using namespace std;

enum FeedLang {
	RSS,
	ATOM
};

class Database {
  private:
	Database();

	sqlite3 *db;

	static const int existed(bool *out, int cols, char *data[], char *colNames[]);

  public:
	Database(const char *filename);
	virtual ~Database();

	void exec(string s) { sqlite3_exec(db, s.c_str(), dummy, NULL, NULL); };

	static int dummy(void *out, int cols, char *data[], char *colNames[]) { return 0; }

	// Putting here so we don't have circular dependancy (Feed <-> Article)
	//TODO: see http://atomenabled.org/developers/syndication/#text
	static const char *parseAtomTitle(const pugi::xml_node &t) { return t.child_value(); };
};

#endif
