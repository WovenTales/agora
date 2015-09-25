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
	struct Table {
	  public:
		//! Encapsulation of information on table columns to allow dynamic generation
		struct Column {
		  private:
			//! Default constructor
			Column();

		  public:
			const std::string name;    //!< Reference name of column (unique within table)
			/*! If different from the enclosing Table::table, indicates a link
			 *    to the \ref column of the same name in the specified table.
			 *
			 *  \todo May want to create secondary reference to actual table
			 *          (table:column identifies in database, name:t2 identifies object)
			 */
			const std::string table;   //!< %Table in which primary column is located

			/*! If included in multiple tables, only one should have \ref table match
			 *    Table::table; that column refered to as "primary" and all other columns
			 *    with the same name should have \ref table identical to the contaning
			 *    Table::table of the *primary* column.
			 */
			const std::string column;  //!< Name in SQLite database (unique within table)
			/*! \todo Make function pointer */
			const bool        update;  //!< Whether to preform simple automatic update on column

			//! Standard constructor
			/*! \param n \copybrief name
			 *  \param t \copybrief table
			 *  \param c \copybrief column
			 *  \param u \copybrief update
			 */
			Column(const std::string &n, const std::string &t, const std::string &c, bool u) :
			                 name(n),              table(t),             column(c),      update(u) {};
		};

		//! List of Table pointers
		typedef std::vector<const Table*> List;

	  private:
		//! Default constructor
		Table();

		//! Name of table in database
		const std::string table;
		//! Main data storage
		const std::unordered_map<std::string, const Column> columns;

		//! Provide means of more easily filling \ref columns
		static std::unordered_map<std::string, const Column> generateMap(const std::vector<Column>&);

		//! Provide a means to hash columns
		struct column_hash_eq {
			//! Hash function
			std::size_t operator() (const Column a) const { return std::hash<std::string>()(a.table + ":" + a.column); };
			//! Equality function
			bool operator() (const Column a, const Column b) const { return ((a.table == b.table) && (a.column == b.column)); };
		};

	  public:
		//! Standard constructor
		/*! \param t    name of the table in the database
		 *  \param data list of Column objects contained in the table
		 */
		Table(const std::string &t, const std::vector<Column> &data) :
			table(t), columns(generateMap(data)) {};

		//! Element access operator
		const Column &operator [](const std::string &ref) const { return columns.at(ref); };

		//! Obtain a Table::Column from a SQLite column name
		const Column &parseColumn(const std::string&) const;
		//! Search the given tables for a Table::Column matching the requested SQLite column name
		static const Column &parseColumn(const List&, const std::string&);
		//! Retrieve the requested Table from the List
		static const Table &parseTable(const List&, const std::string&);

		//! Simplify hash map construction
		/*! \tparam MAPPED_TYPE    the type of object each key references
		 *  \tparam ALLOCATOR_TYPE the type of allocator object for storage
		 */
		template < typename MAPPED_TYPE,
			   typename ALLOCATOR_TYPE = std::allocator<std::pair<const Column, MAPPED_TYPE> > >
		using column_hash_map = std::unordered_map<const Column, MAPPED_TYPE, column_hash_eq, column_hash_eq, ALLOCATOR_TYPE>;
	};

	//! Mappings of (Table::Column) -> (data)
	/*! Represents a lower-level representation of a returned database query.
	 */
	typedef Table::column_hash_map<std::string> Data;
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

	//! Convert SQLite results to a usable DataList
	static int getEntries(DataList*, int, char*[], char*[]);
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

	//! Registration of all tables in database
	static const Table::List tables;
	//! Representation of table \c meta
	static const Table meta;

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

	//! Lookup specified columns in database
	DataList getColumns(const std::initializer_list<Table::Column>&,
	                    const std::initializer_list<std::pair<const Table::Column, std::string> >& = {}) const;

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
//   void clearStaged(std::queue<T*>&);
#include <database.tcc>
};

#endif
