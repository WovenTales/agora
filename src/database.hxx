#ifndef DATABASE_H
#define DATABASE_H

#include <pugixml.hpp>
//TODO: Switch to SOCI wrapper
#include <sqlite3.h>
#include <string>

using namespace std;

enum FeedLang {
	RSS,
	ATOM
};

class Database {
  private:
	friend int main(int,char**);
	Database();

	sqlite3 *db;

  public:
	Database(const char *filename);
	virtual ~Database();

	// Putting here so we don't have circular dependancy (Feed <-> Article)
	//TODO: see http://atomenabled.org/developers/syndication/#text
	static const char *parseAtomTitle(const pugi::xml_node &t) { return t.child_value(); };
};

#endif
