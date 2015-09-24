#ifndef DATABASE_H
#define DATABASE_H

#include <agora.hxx>
class Article;
class Feed;
#include <logger.hxx>

//! \todo Are any of these unnecessary?
#include <algorithm>
#include <initializer_list>
#include <map>
#include <queue>
#include <sqlite3.h>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>


//! An abstraction for a database on file
/*! \todo Update so can have const Database instances (everything complains about exec not being const) */
class Database {
  public:
	//! Column declarations
	struct Table {
	  public:
		friend class Database;

		// When adding new column to any enum, don't forget to add corresponding
		//   name to table in src/database.cxx and object constructor!

		//! Article parameters
		enum struct Article {
			ID,
			FeedID,
			Title,
			Updated,
			Link,
			Author,
			Summary,
			Content
		};
		//! \copybrief column(Meta)
		static std::string column(Article);
		//! \copybrief table(Meta)
		static std::string table(Article) { return "articles"; };

		//! Feed parameters
		enum struct Feed {
			ID,
			URI,
			Title,
			Updated,
			Link,
			Author,
			Description
		};
		//! \copybrief column(Meta)
		static std::string column(Feed);
		//! \copybrief table(Meta)
		static std::string table(Feed) { return "feeds"; };

		//! Meta table parameters
		enum struct Meta {
			Title,
			Version
		};
		//! Retrieve unique referent for SQLite database
		static std::string column(Meta);
		//! Retrieve table name for SQLite database
		static std::string table(Meta) { return "meta"; };

		//! Generic reference to columns
		/*! \tparam T Relevant \b enum of table columns */
		template <typename T> using Unified = T;

	  private:
		//! Provide a means to determine equality of columns
		/*! \copydetails Table::Unified */
		template <typename T>
		struct table_hash_eq {
			//! \cond
			//! Internal use only
			std::size_t operator() (T a) const { return std::hash<int>()(int(a)); };
			//! Internal use only
			bool operator() (T a, T b) const { return (int(a) == int(b)); };
			//! \endcond
		};
		//! Simplify hash map construction
		/*! \todo Document template params */
		template < typename T,
			   typename MAPPED_TYPE,
			   typename ALLOCATOR_TYPE = std::allocator<std::pair<const T, MAPPED_TYPE> > >
		using table_hash_map = std::unordered_map<T, MAPPED_TYPE, table_hash_eq<T>, table_hash_eq<T>, ALLOCATOR_TYPE>;
	};

	//! Mappings of (column name) -> (data)
	/*! Represents a lower-level representation of a returned database query.
	 *  \copydetails Table::Unified
	 */
	template <typename T> using Data     = Table::table_hash_map<Table::Unified<T>, std::string>;
	//! Short name for collecting multiple sets of column Data
	/*! \copydetails Table::Unified */
	template <typename T> using DataList = std::vector<Data<T> >;

	//! \copybrief MetaData
	typedef Data<Table::Article> ArticleData;
	//! \copybrief MetaData
	typedef Data<Table::Feed>    FeedData;
	//! Utility specification of Data
	typedef Data<Table::Meta>    MetaData;

	//! \copybrief MetaDataList
	typedef DataList<Table::Article> ArticleDataList;
	//! \copybrief MetaDataList
	typedef DataList<Table::Feed>    FeedDataList;
	//! Utility specification of DataList
	typedef DataList<Table::Meta>    MetaDataList;

  private:
	//! Encapsulation of information on table columns to allow dynamic generation
	struct ColumnData {
		std::string name;    //!< Name in SQLite database
		/*! \todo Make function pointer */
		bool        update;  //!< Whether to preform simple automatic update on column
	};

	//! Core SQLite3 database pointer
	sqlite3 *db;

	/*! \todo Move to respective objects, along with column struct */
	//! \copybrief MetaColumnName
	static const Table::table_hash_map<Database::Table::Article, Database::ColumnData> ArticleColumnName;
	//! \copybrief MetaColumnName
	static const Table::table_hash_map<Database::Table::Feed, Database::ColumnData>    FeedColumnName;
	//! Provide a lookup for SQLite database names
	static const Table::table_hash_map<Database::Table::Meta, Database::ColumnData>    MetaColumnName;

	//! List of changes to save to the database
	std::queue<const Article*> articleStage;
	//! \copybrief articleStage
	std::queue<const Feed*>    feedStage;

	//! Number of references to the database
	/*! Allows multiple copies without leaving scope. */
	unsigned char &count;

	//! Close database
	void close(bool = true);

	//! Execute a command on the database
	void exec(const std::string&);

	//! Tests if a SQLite command returns no matching data
	static int isEmpty(bool*, int, char*[], char*[]);

  public:
	//! Initialize without physical database
	Database();
	//! Initialize to a particular file
	Database(const std::string &);
	//! Copy constructor
	Database(const Database&);
	//! Standard destructor
	virtual ~Database();

	//! Assignment operator
	Database &operator=(const Database&);

	//! Get title assigned to database
	std::string getTitle() const;

	//! Request an Article by ID
	Article getArticle(const std::string&) const;
	//! Request a Feed by ID
	Feed getFeed(const std::string&) const;
	//! Check feed document and update if necessary
	void updateFeed(const std::string&);

	//! Open specified database file
	void open(const std::string&);

	//! Stage a Feed for writing
	void stage(const Feed&);
	//! Stage an Article for writing
	void stage(const Article&);

	//! Clear all staged changes without saving
	void clearStaged();
	//! Commit the staged changes to the file
	void save();
	/*! \class Database
	 *  \todo Add method for removing and/or changing staged elements\n
	 *  Can likely best accomplish at same time as preventing multiple elements with same ID
	 */

// private:
//   static T parseColumn(std::string);
//   void clearStaged(std::queue<T*>&);
//   static int getEntries(Database::DataList<T>*, int, char*[], char*[]);
// public:
//   DataList<T> getColumns(const std::initializer_list<Table::Unified<T> >&,
//                          const std::initializer_list<std::pair<Table::Unified<T>, std::string> >&) const {
#include <database.tcc>
};

#endif
