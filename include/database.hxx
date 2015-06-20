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
//! \todo Update so can have const Database instances (everything complains about exec not being const).
//! \todo Public \c exec exposes too much control; encapsulate with safer get methods.
class Database {
  private:
	sqlite3 *db;

	std::queue<const Feed*> feedStage;
	std::queue<const Article*> articleStage;

	unsigned char &count;

	Database &operator--();

	//! Close database.
	void close(bool = false);

	// For use in sqlite3_exec() calls
	static int getEntries(std::vector<std::map<std::string, std::string> >*, int, char*[], char*[]);
	static int isEmpty(bool*, int, char*[], char*[]);

  public:
	//! Initialize without physical database.
	Database();
	//! Initialize to a particular file.
	Database(const std::string &);
	//! Copy constructor.
	Database(const Database&);
	//! Standard deconstructor.
	virtual ~Database();

	//! Standard assignment operator.
	Database &operator=(const Database&);

	//! Mappings of (columnName) -> (data), representing a lower-level representation of an Article.
	//! \todo Make enum or similar out of column names, so don't have to worry about exact implementation
	typedef typename std::map<std::string, std::string> Data;
	typedef typename std::vector<Data>                  DataList;

	//! Get title assigned to database.
	std::string getTitle();

	//! Request an Article by ID.
	Article        getArticle(const std::string&);
	//! Create Article from given data.
	static Article makeArticle(const Data);

	//! Open specified database file.
	void open(const std::string&);

	//! Stage a Feed for writing.
	void stage(const Feed&);
	//! Stage an Article for writing.
	void stage(const Article&);

	//! Clear all staged changes without saving.
	void clearStaged();
	//! Commit the staged changes to the file.
	void save();
	/*! \class Database
	 *  \todo Add method for removing and/or changing staged elements\n
	 *  Can likely best accomplish at same time as preventing multiple elements with same ID
	 */

	//! Execute a command on the database.
	void      exec(const std::string&);
	//! Execute a command on the database, returning resulting data.
	DataList *exec(const std::string&, int);

// private: void clearStaged(std::queue<T*>&);
#include <database.tcc>
};

#endif
