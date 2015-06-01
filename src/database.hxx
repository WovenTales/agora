#ifndef DATABASE_H
#define DATABASE_H

class Article;
class Feed;

#include <sqlite3.h>
#include <queue>
#include <string>

using namespace std;

class Database {
  private:
	Database();

	sqlite3 *db;

	std::queue<Feed> feedStage;
	std::queue<Article*> articleStage;

	static int existed(bool *out, int cols, char *data[], char *colNames[]);
	static int makeArticle(Article*, int, char**, char**);

  public:
	Database(const char *filename);
	virtual ~Database();

	void exec(const std::string &s) { sqlite3_exec(db, s.c_str(), NULL, NULL, NULL); };

	Article getArticle(const std::string&);

	void stage(const Feed &f);
	void stage(Article *a) { articleStage.push(a); };

	void save();
};

#endif
