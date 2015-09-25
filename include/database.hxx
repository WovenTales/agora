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
	//! Lookup table for info on tables in database
	struct ColumnWrapper {
	  public:
		//! Encapsulation of information on table columns to allow dynamic generation
		struct ColumnData {
		  private:
			//! Default constructor
			ColumnData();

		  public:
			/*! \warning Not necessarily unique; also consider \ref table to guarentee uniqueness. */
			const std::string name;    //!< Reference name of column
			/*! If different from the enclosing ColumnWrapper::table, indicates a link
			 *    to the column of the same name in the specified table.
			 *
			 *  \todo May want to create secondary reference to actual table
			 *          (table:column identifies in database, name:t2 identifies object)
			 */
			const std::string table;   //!< Table in which primary column is located

			const std::string column;  //!< Name in SQLite database
			/*! \todo Make function pointer */
			const bool        update;  //!< Whether to preform simple automatic update on column

			//! Standard constructor
			/*! \todo Find way of linking ColumnData::name to ColumnWrapper::columns.first automatically
			 */
			ColumnData(const std::string &n, const std::string &t, const std::string &c, bool u) :
			                 name(n),              table(t),             column(c),      update(u) {};
		};

	  private:
		//! Default constructor
		ColumnWrapper();

		//! Name of table in database
		const std::string table;
		//! Main data storage
		const std::unordered_map<std::string, const ColumnData> columns;

		//! Provide means of more easily filling \ref columns
		std::unordered_map<std::string, const ColumnData> &generateMap(const std::vector<ColumnData>&);

		//! Provide a means to hash columns
		struct column_hash_eq {
			//! Hash function
			std::size_t operator() (ColumnData a) const { return std::hash<std::string>()(a.table + ":" + a.column); };
			//! Equality function
			bool operator() (ColumnData a, ColumnData b) const { return ((a.table == b.table) && (a.column == b.column)); };
		};

	  public:
		ColumnWrapper(const std::string &t, const std::vector<ColumnData> &data) :
			table(t), columns(generateMap(data)) {};

		//! Element access operator
		const ColumnData &operator [](const std::string &ref) { return columns.at(ref); };

		//! Simplify hash map construction
		/*! \tparam MAPPED_TYPE    the type of object each key references
		 *  \tparam ALLOCATOR_TYPE the type of allocator object for storage
		 */
		template < typename MAPPED_TYPE,
			   typename ALLOCATOR_TYPE = std::allocator<std::pair<const ColumnData, MAPPED_TYPE> > >
		using column_hash_map = std::unordered_map<ColumnData, MAPPED_TYPE, column_hash_eq, column_hash_eq, ALLOCATOR_TYPE>;
	};

	//! Mappings of (ColumnWrapper::ColumnData) -> (data)
	/*! Represents a lower-level representation of a returned database query.
	 */
	typedef ColumnWrapper::column_hash_map<std::string> Data;
	//! Short name for collecting multiple sets of column Data
	typedef std::vector<Data> DataList;

  private:
	//! Core SQLite3 database pointer
	sqlite3 *db;

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

	//! Representation of table \c meta
	static const ColumnWrapper columns;

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
