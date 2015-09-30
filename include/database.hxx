#ifndef DATABASE_H
#define DATABASE_H

#include <agora.hxx>
class Article;
class Feed;
#include <logger.hxx>

#include <queue>
#include <sqlite3.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


//! An abstraction for a database on file
/*! \todo Update so can have const Database instances (everything complains about exec not being const) */
class Database {
  public:
	//! Lookup table for info on tables in database
	struct Table {
	  public:
		//! List of Table pointers
		typedef std::vector<const Table*> List;

		//! Encapsulation of information on table columns to allow dynamic generation
		struct Column {
		  private:
			friend Database::Table;

			//! Default constructor
			Column();

			//! Constructor to add reference to Table
			Column(const Column &c, const Database::Table *t) :
				name(c.name), column(c.column), update(c.update), table(t) {};

		  public:
			//! Standard constructor
			/*! \param n \copybrief name
			 *  \param c \copybrief column
			 *  \param u \copybrief update
			 *
			 *  \warning The resulting Column is the proper format to insert into a
			 *             Database::Table, but modifications as it is added result in an
			 *             object that is not identical to this original construction.
			 */
			Column(const std::string &n, const std::string &c, bool u) :
			             name(n),              column(c),      update(u), table(NULL) {};

			const std::string             name;    //!< Reference name of column (unique within table)
			const Database::Table * const table;   //! The containing Database::Table

			const std::string             column;  //!< Name in SQLite database (unique within table)
			/*! \todo Make function pointer */
			const bool                    update;  //!< Whether to preform simple automatic update on column

			//! Equality operator
			bool operator==(const Column &r) const { return ((table == r.table) && !column.compare(r.column)); };  // table == r.table && column == r.column
		};

	  private:
		//! Default constructor
		Table();

		//! Column for unique row identification
		const Column * const id;
		//! Main data storage
		/*! \todo Encapsulate and expose so table.columns and table.links function identically */
		const std::unordered_map<std::string, const Column> columns;

		//! Provide means of more easily filling \ref columns
		static std::unordered_map<std::string, const Column> generateMap(const Table*, const std::vector<Column>&);
		//! Provide means of initializing \ref id
		static const Column *makeID(const Table*, const std::string&, bool);

		//! Provide a means to hash columns
		struct column_hash_eq {
			//! Hash function
			std::size_t operator() (const Column a) const { return std::hash<std::string>()((a.table ? a.table->title : "") + ":" + a.column); };
			//! Equality function
			bool operator() (const Column a, const Column b) const { return (((a.table ? a.table->title : "") == (b.table ? b.table->title : "")) && (a.column == b.column)); };
		};

	  public:
		//! Standard constructor
		/*! \todo Prevent linking any tables without ID
		 *
		 *  \param t    name of the table in the database
		 *  \param data list of Column objects contained in the table
		 */
		Table(const std::string &t, bool u,                 const std::unordered_set<const Table*> &l, const std::vector<Column> &data) :
		            title(t),       id(makeID(this, t, u)),       links(l),                                  columns(generateMap(this, data)) {};

		//! Standard destructor
		virtual ~Table() { if (id != NULL) delete id; };

		//! Name of table in database
		const std::string title;
		//! Secondary data to link to in other Table objects
		/*! \todo Encapsulate so exact representation doesn't matter */
		const std::unordered_set<const Table*> links;

		//! Whether the table has a unique ID for each row
		const bool hasID() const { return (id != NULL); };
		//! Get unique ID of the table
		const Column &getID() const { return *id; };

		//! Element access operator
		/*! \todo More gracefully and verbosely handle "No such column" */
		const Column &operator [](const std::string &ref) const { return columns.at(ref); };
		//! Iterator to first column in internal order
		std::unordered_map<std::string, const Column>::const_iterator iterator() const { return columns.cbegin(); };
		//! Iterator to last column in internal order
		std::unordered_map<std::string, const Column>::const_iterator end() const { return columns.cend(); };

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
	static const Table::List tableList;
	//! Representation of table \c meta
	static const Table table;

	//! Get title assigned to database
	std::string getTitle() const;

	//! Request an Article by ID
	Article getArticle(const std::string&) const;
	//! Request a Feed by ID
	Feed getFeed(const std::string&) const;
	//! Check feed document and update if necessary
	Feed updateFeed(const Feed&);

	//! Open specified database file
	void open(const std::string&);

	//! Lookup specified columns in database
	DataList getColumns(const std::vector<Table::Column>&, const std::vector<std::pair<const Table::Column, std::string> >& = {}) const;

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
