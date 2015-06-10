#ifndef DATABASE_H
#define DATABASE_H

class Article;
class Feed;

#include <sqlite3.h>
#include <queue>
#include <string>


//! An abstraction for a database on file.
class Database {
  private:
	Database();

	sqlite3 *db;

	std::queue<const Feed*> feedStage;
	std::queue<const Article*> articleStage;

	// For use in sqlite3_exec() calls
	static int isEmpty(bool *out, int cols, char *data[], char *colNames[]);
	static int makeArticle(Article**, int, char**, char**);

  public:
	//! Initialize to a particular file.
	Database(const std::string &);
	//! Standard deconstructor.
	virtual ~Database();

	//! Request an article by ID.
	Article *getArticle(const std::string&);

	//! Stage a Feed for writing.
	void stage(const Feed &f);
	//! Stage an Article for writing.
	void stage(const Article &a);

	//! Commit the staged changes to the file.
	void save();
	/*! \class Database
	 *  \todo Add method for removing and/or changing staged elements\n
	 *  Can likely best accomplish at same time as preventing multiple elements with same ID
	 */

	//! Execute a command on the database.
	/*! \param s SQLite3 command to execute
	 */
	void exec(const std::string &s) { sqlite3_exec(db, s.c_str(), NULL, NULL, NULL); };
};

#endif
