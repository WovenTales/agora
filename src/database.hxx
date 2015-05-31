#ifndef DATABASE_H
#define DATABASE_H

class Article;

#include <pugixml.hpp>
#include <sqlite3.h>
#include <string>

using namespace std;

class Database {
  private:
	Database();

	sqlite3 *db;

	static const int existed(bool *out, int cols, char *data[], char *colNames[]);

  public:
	Database(const char *filename);
	virtual ~Database();

	void exec(std::string s) { sqlite3_exec(db, s.c_str(), NULL, NULL, NULL); };

	Article getArticle(std::string);

	static int makeArticle(Article*, int, char**, char**);

	//TODO: see http://atomenabled.org/developers/syndication/#text
	static const char *parseAtomTitle(const pugi::xml_node &t) { return t.child_value(); };
	static time_t parseTime(const char*);
	static time_t parseTime(std::string);
};

#endif
