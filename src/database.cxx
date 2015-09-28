#include <database.hxx>

#include <article.hxx>
#include <feed.hxx>

#include <iostream>
#include <regex>
#include <string.h>

using namespace agora;
using namespace pugi;
using namespace std;



/*! Note that Meta columns are not updated, and so that column has no effect.
 */
const Database::Table Database::meta("meta", false, {}, {
	{ "title",   "title",   false },
	{ "version", "version", false }
	});

const Database::Table::List Database::tables = { &Database::meta, &Article::columns, &Feed::columns };



/*! Essentially, extracts Column.name from each object, and files it in the map
 *    under that same string.
 *
 *  \todo Ensure uniqueness of proper columns
 *
 *  \param data List of Column objects with which to populate the map
 *
 *  \return a map prepared for direct assignment to \ref columns
 */
std::unordered_map<std::string, const Database::Table::Column> Database::Table::generateMap(const Database::Table *table, const std::vector<Database::Table::Column> &data) {
	unordered_map<string, const Column> out;

	for (Column c : data) {
		out.emplace(c.name, Column(c, table));
	}

	return out;
}

const Database::Table::Column *Database::Table::makeID(const Table *table, const std::string &title, bool unique) {
	if (!unique) {
		return NULL;
	} else {
		//! \todo Remarkably inefficiant
		return new Column(Column("id", ("_" + title + "_ID"), false), table);
	}
}


/*! \param col SQLite name to parse
 *
 *  \return a Table::Column representing the column with the given SQLite name
 */
const Database::Table::Column &Database::Table::parseColumn(const std::string &col) const {
	Log << "Searching for column '" << col << "' in table '" << title << "'...";

	if (hasID()) {
		const Column &id = getID();
		if (!id.column.compare(col)) {  // col == table ID
			return id;
		}
	}

	auto lend = links.cend();
	for (auto it = links.cbegin(); it != lend; ++it) {
		const Column &lid = (*it)->getID();
		if (!lid.column.compare(col)) {  // col == linked table ID
			return lid;
		}
	}

	const Column *out = NULL, *c;
	auto cend = end();
	for (auto it = iterator(); it != cend; ++it) {
		c = &(it->second);

		if (!c->column.compare(col)) {  // c.column == col
			if (out != NULL) {
				//! \todo Throw error: multiple columns with same SQLite name
			}
			out = c;
		}
	}

	if (out == NULL) {
		//! \todo Throw error: no such column found
		Log << "No such column found" << Log.ENDL;
	} else {
		Log << "Found" << Log.ENDL;
	}

	return *out;
}

/*! If the column is linked in multiple tables, will return the primary column.
 *
 *  \param list the List of tables in which to search
 *  \param col  SQLite name to parse
 *
 *  \return a Table::Column representing the primary column with the given SQLite name
 */
const Database::Table::Column &Database::Table::parseColumn(const Database::Table::List &list, const std::string &col) {
	Log << "Searching for column '" << col << "' in list of tables..." << Log.ENDL;
	const Column *out = NULL, *c;

	for (const Table *t : list) {
		Log << Log.CONTINUE;
		//! \todo Ensure that catching No such column exception once implemented, and set c = NULL in that case
		c = &(t->parseColumn(col));

		if (c != NULL) {
			if (out == NULL) {
				out = c;
			} else {  // out.title != c.title
				//! \todo Throw error: multiple columns with same SQLite name
			}
		}
	}

	if (out == NULL) {
		//! \todo Throw error: no such column found
		Log << Log.CONTINUE << "No such column found" << Log.ENDL;
	} else {
		Log << Log.CONTINUE << "Found primary column in table '" << out->table->title << "'" << Log.ENDL;
	}

	return *out;
}

/*! \param list  the List of tables is which to search
 *  \param table the table to search for
 *
 *  \return a Table representing that with the given SQLite name
 */
const Database::Table &Database::Table::parseTable(const Database::Table::List &list, const std::string &table) {
	Log << "Searching for table '" << table << "'...";
	const Table *out = NULL;

	for (const Table *t : list) {
		if (!t->title.compare(table)) {  // name(t) == table
			if (out != NULL) {
				//! \todo Throw error: multiple identically-named tables
			}
			out = t;
		}
	}

	if (out == NULL) {
		//! \todo Throw error: no such table found
		Log << "No such error found" << Log.ENDL;
	} else {
		Log << "Found" << Log.ENDL;
	}

	return *out;
}



Database::Database() : db(NULL), count(*new unsigned char(1)) {
	open(":memory:");
}

/*! \param filename the SQLite3 database to associate this Database with
 */
Database::Database(const std::string &filename) : db(NULL), count(*new unsigned char(1)) {
	open(filename);
}

/*! \param d the Database to copy
 */
Database::Database(const Database &d) : db(d.db), count(d.count) {
	++count;
}

Database::~Database() {
	if (--count == 0) {
		close();

		delete &count;
	}
}


/*! Changes pointed database, but doesn't duplicate staged elements.
 *
 *  \warning Discards any items staged for the old database.
 *           Call Database::save() before assignment to avoid loss of data.
 */
Database &Database::operator=(const Database &d) {
	// Clean up previous object
	if (--count == 0) {
		// Force discard as speed is more important in this situation
		close(false);
	} else {
		clearStaged();
	}

	count = ++d.count;
	db    = d.db;
}


/*! If database doesn't contain given ID, returns an Article with default values
 *    (mostly empty strings) for all members.
 *
 *  \param id article ID
 *  \return An Article with values according to given ID
 */
Article Database::getArticle(const std::string &id) const {
	Log << "Requesting article with id '" << id << "'..." << (Log.ENDL | Log.CONTINUE);

	DataList rows = getColumns({}, {{ Article::columns.getID(), id }});

	if (!rows.size()) {  // rows.size() == 0
		Log << Log.CONTINUE << "No such article found" << Log.ENDL;
		return Article();
	} else {
		Log << Log.CONTINUE << "Found article matching id" << Log.ENDL;
		return Article(rows[0]);
	}
}

/*! If database doesn't contain given ID, returns a Feed with default values
 *    (mostly empty strings) for all members.
 *
 *  \param id feed ID
 *  \return An Feed with values according to given ID
 */
Feed Database::getFeed(const std::string &id) const {
	Log << "Requesting feed with id '" << id << "'..." << (Log.ENDL | Log.CONTINUE);

	DataList rows = getColumns({}, {{ Feed::columns.getID(), id }});

	if (!rows.size()) {  // rows.size() == 0
		Log << Log.CONTINUE << "No such feed found" << Log.ENDL;
		return Feed();
	} else {
		Log << Log.CONTINUE << "Found feed matching id" << Log.ENDL;
		return Feed(rows[0]);
	}
}

/*! \todo Be smarter in determining what needs to be updated in feed.\n
 *        Might not provide any benefit as we have already downloaded and constructed it.
 *
 *  \param id the id of the feed to update
 */
void Database::updateFeed(const std::string &id) {
	Feed local = getFeed(id);

	// Dumb copy over works as long as no modified data is stored in main table
	stage(Feed(local.getURI(), local.getLang()));
}


/*! Internal function as improper use could leave object in a fragile state.
 *
 *  \param commit whether to commit changes or discard them (default: save)
 */
void Database::close(bool commit) {
	if (db) {
		Log << "Closing database" << Log.ENDL;

		if (commit) {
			clearStaged();
		} else {
			save();
		}
		sqlite3_close(db);

		db = NULL;
	}
}


/*! If object already has an active database, saves changes to that and closes it rather than overwriting.
 *
 *  \param filename the feed database to associate this Database with
 */
void Database::open(const std::string &filename) {
	// Properly handle any existing database
	if (!db) {
		close();
	}

	Log << "Opening database '" << filename << "'...";
	if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
		//! \todo Better error handling
		cerr << "Bad database file" << endl;
	}

	Log << "Completed" << Log.ENDL;

	bool init = true;
	// If this is a new database, table is nonexistent and so table_info contains no columns
	sqlite3_exec(db, "PRAGMA table_info(meta)", (int (*)(void*, int, char**, char**))isEmpty, &init, NULL);

	if (init) {
		string command("");

		for (const Table *t : tables) {
			command.append("CREATE TABLE " + t->title + " (");

			if (t->hasID()) {
				command.append(t->getID().column + " NOT NULL PRIMARY KEY ON CONFLICT REPLACE,");
			}

			bool firstCol = true;
			auto lend = t->links.cend();
			for (auto it = t->links.cbegin(); it != lend; ++it) {
				command.append((firstCol ? "" : ",") + (*it)->getID().column);
				firstCol = false;
			}

			auto end = t->end();
			for (auto it = t->iterator(); it != end; ++it) {
				command.append((firstCol ? "" : ",") + it->second.column);
				firstCol = false;
			}

			command.append(");");
		}

		//! \todo Another good candidate for parametric data storage
		command.append("INSERT INTO meta (title, version) VALUES ('" + replaceAll(filename, "'", "''") + "',3);");

		exec(command.c_str());
		Log << "New database initialized" << Log.ENDL;
	}
	//! \todo Allow updating old database format
}


/*! If \c cols is an empty initializer_list, selects all columns matching \c where.
 *  Any columns in \c where that aren't in the same table as anything in \c cols are ignored.
 *
 *  \bug As it is, this might only allow a single T (i.e. only columns from a single table, rather than combining them)
 *
 *  \param cols  list of \link Database::Table Database::Table::Column\endlink objects to select
 *  \param where pairs of (\link Database::Table Database::Table::Column\endlink, match_string); optional unless \c cols is empty
 */
Database::DataList Database::getColumns(const std::initializer_list<Database::Table::Column> &cols,
                                        const std::initializer_list< std::pair<const Database::Table::Column, std::string> > &where) const {
	Log << "Constructing SELECT request...";
	DataList out;

	// If no restrictions are passed
	if (!cols.size() && !where.size()) {
		return out;
	}

	vector<string> tables;

	string cmdColumn("");
	string cmdTables("");
	string cmdWhere("");

	// For each column requested
	bool firstCol = true;
	string t;
	for (Table::Column c : cols) {
		t = c.table->title;

		// If c's containing table is not already in tables
		if (find(tables.begin(), tables.end(), t) == tables.end()) {
			tables.push_back(t);
		}

		// Add column to command
		if (firstCol) {
			cmdColumn = c.column;
			firstCol = false;
		} else {
			cmdColumn.append("," + c.column);
		}
	}

	// For each search restriction
	firstCol = true;
	for (pair<const Table::Column, string> w : where) {
		t = w.first.table->title;

		// Whether any particular columns were requested
		bool colsEmpty = (cols.size() == 0);
		// Whether w's containing table is in tables
		//! \todo Ensure proper handling of linked columns
		bool tInTables = (find(tables.begin(), tables.end(), t) != tables.end());

		// Block only relevant if w refers to an involved table, or if we're wanting entire table
		if (colsEmpty || tInTables) {
			// We're selecting all columns, but t isn't yet being requested
			if (colsEmpty && !tInTables) {
				tables.push_back(t);
			}

			// Add restriction to command
			if (firstCol) {
				cmdWhere = " WHERE ";
				firstCol = false;
			} else {
				cmdWhere.append(" AND ");
			}
			cmdWhere.append(w.first.column + " = '" + agora::replaceAll(w.second, "'", "''") + "'");
		}
	}

	// Join all involved tables
	firstCol = true;
	for (string ta : tables) {
		if (firstCol) {
			cmdTables = ta;
			firstCol = false;
		} else {
			cmdTables.append(" NATURAL JOIN " + ta);
		}
	}

	string cmd = "SELECT " + (cols.size() ? cmdColumn : "*") + " FROM " + cmdTables + cmdWhere + ";";

	// Execute the compiled command
	Log << "Completed" << (Log.ENDL | Log.CONTINUE) << "Executing command..." << (Log.ENDL | Log.CONTINUE);
	//! \todo Implement ability to pause log to prevent spam
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))getEntries, &out, NULL);
	// Log << "Completed" << Log.ENDL;
	return out;
}


void Database::clearStaged() {
	Log << "Clearing feeds...";
	clearStaged(feedStage);
	Log << "Completed" << Log.ENDL;

	Log << "Clearing articles...";
	clearStaged(articleStage);
	Log << "Completed" << Log.ENDL;
}

/*! Creates a copy of the article to avoid worry about scope.
 *
 *  \param a the article to stage
 */
void Database::stage(const Article &a) {
	// Create a new instance so user doesn't have to worry about scope
	Article *p = new Article(a);
	Log << "Staging article '" << p->getTitle() << "'...";

	articleStage.push(p);
	Log << "Completed" << Log.ENDL;
}

/*! Creates a copy of the feed to avoid worry about scope.
 *
 *  \param f the feed to stage
 */
void Database::stage(const Feed &f) {
	Feed *p = new Feed(f);
	Log << "Staging feed '" << p->getTitle() << "'..." << (Log.ENDL | Log.CONTINUE);

	feedStage.push(p);

	// Generate and stage articles
	string id = p->getID();
	FeedLang lang = p->getLang();
	xml_node root = p->getRoot();
	const char *tag = (lang == ATOM ? "entry" : "item");
	unsigned int count = 0;

	for (xml_node entry = root.child(tag);
	     entry.type(); // != NULL
	     entry = entry.next_sibling(tag)) {
		Article a(entry, *p, lang);
		Log << Log.CONTINUE;

		stage(a);
		Log << Log.CONTINUE;
		++count;
	}

	Log << "Staged " << count << " articles from feed" << Log.ENDL;
}


//! \todo Update to use new Table::Column structure
void Database::save() {
	Log << "Committing staged elements..." << (Log.ENDL | Log.CONTINUE);

	unsigned int countF = 0, countA = 0;
	string insert("");
	string cols("");
	string vals("");

	// Essentially, retrieve the next element and form a SQLite command out of each member, then repeat
	const Article *a;
	while (!articleStage.empty()) {
		a = articleStage.front();
		articleStage.pop();

		string id      = replaceAll(a->getID(), "'", "''");
		string feedID  = replaceAll(a->getFID(), "'", "''");
		string title   = replaceAll(a->getTitle(), "'", "''");
		string link    = replaceAll(a->getLink(), "'", "''");
		string author  = replaceAll(a->getAuthor(), "'", "''");
		string summary = replaceAll(a->getSummary(), "'", "''");
		string content = replaceAll(a->getContent(), "'", "''");

		Log << "Generating command for article '" << title << "'...";

		cols = Article::columns.getID().column + ", " + Feed::columns.getID().column;
		vals = "'" + id + "','" + feedID + "'";

		if (title.compare("")) {  // title != ""
			cols += ", aTitle";
			vals += ",'" + title + "'";
		}
		if (link.compare("")) {  // link != ""
			cols += ", aLink";
			vals += ",'" + link + "'";
		}
		if (author.compare("")) {  // author != ""
			cols += ", aAuthor";
			vals += ",'" + author + "'";
		}
		if (summary.compare("")) {  // summary != ""
			cols += ", aSummary";
			vals += ",'" + summary + "'";
		}
		if (content.compare("")) {  // content != ""
			cols += ", aContent";
			vals += ",'" + content + "'";
		}

		//! \todo Only update what's necessary rather than replacing everything
		insert += "INSERT INTO articles (" + cols + ") VALUES (" + vals + ");";

		delete a;
		countA++;

		Log << "Completed" << (Log.ENDL | Log.CONTINUE);
	}

	const Feed *f;
	while (!feedStage.empty()) {
		f = feedStage.front();
		feedStage.pop();

		string id          = replaceAll(f->getID(), "'", "''");
		string uri         = replaceAll(f->getURI(), "'", "''");
		string title       = replaceAll(f->getTitle(), "'", "''");
		string link        = replaceAll(f->getLink(), "'", "''");
		string author      = replaceAll(f->getAuthor(), "'", "''");
		string description = replaceAll(f->getDescription(), "'", "''");

		Log << "Generating command for feed '" << title << "'...";

		cols = Feed::columns.getID().column;
		vals = "'" + id + "'";

		if (uri.compare("")) {  // uri != ""
			cols += ", fURI";
			vals += ",'" + uri + "'";
		}
		if (title.compare("")) {  // title != ""
			cols += ", fTitle";
			vals += ",'" + title + "'";
		}
		if (link.compare("")) {  // link != ""
			cols += ", fLink";
			vals += ",'" + link + "'";
		}
		if (author.compare("")) {  // author != ""
			cols += ", fAuthor";
			vals += ",'" + author + "'";
		}
		if (description.compare("")) {  // description != ""
			cols += ", fDesc";
			vals += ",'" + description + "'";
		}

		//! \todo Only update what's necessary rather than replacing everything
		insert += "INSERT INTO feeds (" + cols + ") VALUES (" + vals + ");";

		if (f->getCount() == 1) {
			delete f;
		}
		countF++;

		Log << "Completed" << (Log.ENDL | Log.CONTINUE);
	}

	exec(insert);

	Log << "Saved " << countA << " articles and " << countF << " feeds from stage" << Log.ENDL;
}


/*! \param cmd SQLite3 command to execute
 */
void Database::exec(const std::string &cmd) {
	//! \todo Implement DEBUG level in log
	//Log << "Executing command '" << cmd << "'" << Log.ENDL;
	sqlite3_exec(db, cmd.c_str(), NULL, NULL, NULL);
}


/*! For use in sqlite3_exec() calls.
 *
 *  \todo Document params
 */
int Database::getEntries(Database::DataList *out, int cols, char *data[], char *colNames[]) {
	Data m;
	for (int i = 0; i < cols; i++) {
		m[Table::parseColumn(tables, colNames[i])] = (data[i] ? agora::replaceAll(data[i], "''", "'") : "");
		Log << Log.CONTINUE;
	}

	out->push_back(m);
	Log << Log.ENDL;
	return 0;  // Successful call
};

/*! For use in sqlite3_exec() calls.
 *
 *  \todo Document params
 */
int Database::isEmpty(bool *out, int cols, char *data[], char *colNames[]) {
	// If no matching columns are found
	//! \bug Does this hold for WHERE clauses matching nothing like it does for PRAGMA?
	if (cols == 0) {
		*out = true;
	} else {
		*out = false;
	}
	return 0;  // Successful call
}
