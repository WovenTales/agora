#ifndef DATABASE_H
#define DATABASE_H

class Article;
class Feed;

#include <map>
#include <queue>
#include <sqlite3.h>
#include <string>
#include <vector>


//! An abstraction for a database on file.
class Database {
  private:
	sqlite3 *db;

	std::queue<const Feed*> feedStage;
	std::queue<const Article*> articleStage;

	// For use in sqlite3_exec() calls
	static int getEntries(std::vector<std::map<std::string, std::string> >*, int, char*[], char*[]);
	static int isEmpty(bool*, int, char*[], char*[]);

  public:
	//! Initialize without physical database.
	Database();
	//! Initialize to a particular file.
	Database(const std::string &);
	//! Standard deconstructor.
	virtual ~Database();

	//! Mappings of (columnName) -> (data), representing a lower-level representation of an Article.
	//! \todo Make enum or similar out of column names, so don't have to worry about exact implementation
	typedef typename std::map<std::string, std::string> ArticleData;

	//! Request an Article by ID.
	Article getArticle(const std::string&);
	//! Create Article from given data.
	static Article makeArticle(const ArticleData);

	//! Close database.
	void close(bool = false);
	//! Open specified database file.
	void open(const std::string &);

	//! Stage a Feed for writing.
	void stage(const Feed &f);
	//! Stage an Article for writing.
	void stage(const Article &a);

	//! Clear all staged changes without saving.
	void clearStaged();
	//! Commit the staged changes to the file.
	void save();
	/*! \class Database
	 *  \todo Add method for removing and/or changing staged elements\n
	 *  Can likely best accomplish at same time as preventing multiple elements with same ID
	 */

	//! Execute a command on the database.
	void                      exec(const std::string &s);
	//! Execute a command on the database, returning resulting data.
	std::vector<ArticleData> *exec(const std::string &s, int);

#include <database.tcc>
};

#endif
