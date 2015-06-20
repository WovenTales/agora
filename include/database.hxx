#ifndef DATABASE_H
#define DATABASE_H

class Article;
class Feed;

#include <initializer_list>
#include <queue>
#include <sqlite3.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


//! An abstraction for a database on file.
//! \todo Update so can have const Database instances (everything complains about exec not being const).
//! \todo Public \c exec exposes too much control; encapsulate with safer get methods.
class Database {
  public:
	struct Column {
	  private:
		struct Name {
			Name(const std::string &t, const std::string &c) { table = t; column = c; };

			std::string table;
			std::string column;

			struct Hash {
				std::size_t operator()(const Name &n) const {
					return (std::hash<std::string>()(n.table) ^ (std::hash<std::string>()(n.column) << 1));
				};
			};
			bool operator==(const Name &r) const {
				return ((table == r.table) && (column == r.column));
			}
		};
		friend Database;

	  public:
		static Name DBTitle;
		static Name DBVersion;

		static Name ArticleID;
		static Name ArticleTitle;
		static Name ArticleUpdated;
		static Name ArticleLink;
		static Name ArticleAuthor;
		static Name ArticleSummary;
		static Name ArticleContent;

		static Name FeedID;
		static Name FeedTitle;
		static Name FeedLink;
		static Name FeedAuthor;
		static Name FeedDescription;

		static Name parse(const std::string&);
	};

	//! Mappings of (columnName) -> (data), representing a lower-level representation of an Article.
	//! \todo Make enum or similar out of column names, so don't have to worry about exact implementation
	typedef typename std::unordered_map<Column::Name, std::string, Column::Name::Hash> Data;
	typedef typename std::vector<Data> DataList;

  private:
	Database &operator--();

	sqlite3 *db;

	std::queue<const Feed*> feedStage;
	std::queue<const Article*> articleStage;

	unsigned char &count;

	//! Close database.
	void close(bool = false);

	//! Execute a command on the database.
	void exec(const std::string&);

	// For use in sqlite3_exec() calls
	static int getEntries(std::vector<Data>*, int, char*[], char*[]);
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

	DataList getColumns(const std::initializer_list<Column::Name>&,
	                    const std::initializer_list<std::pair<Column::Name, std::string> >& = {}) const;
	//! Get title assigned to database.
	std::string getTitle() const;

	//! Request an Article by ID.
	Article        getArticle(const std::string&) const;
	//! Create Article from given data.
	static Article makeArticle(const Data&);

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

// private: void clearStaged(std::queue<T*>&);
#include <database.tcc>
};

#endif
