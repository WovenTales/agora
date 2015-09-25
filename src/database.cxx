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
const Database::Table Database::meta("meta", {
	{ "title",   "meta", "title",   false },
	{ "version", "meta", "version", false }
	});

const Database::Table::List Database::tables = { &Database::meta, &Article::columns, &Feed::columns };



/*! Essentially, extracts Column.name from each object, and files it in the map
 *    under that same string.
 *
 *  \param data List of Column objects with which to populate the map
 *
 *  \return a map prepared for direct assignment to \ref columns
 */
std::unordered_map<std::string, const Database::Table::Column> Database::Table::generateMap(const std::vector<Database::Table::Column> &data) {
	unordered_map<string, const Column> out;

	for (Column c : data) {
		out.emplace(c.name, c);
	}

	return out;
}


/*! \param col SQLite name to parse
 *
 *  \return a Table::Column representing the column with the given SQLite name
 */
const Database::Table::Column &Database::Table::parseColumn(const std::string &col) const {
	const Column *out = NULL, *c;

	auto end = columns.cend();
	for (auto it = columns.cbegin(); it != end; ++it) {
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
	const Column *out = NULL, *c;

	for (const Table *t : list) {
		//! \todo Ensure that catching No such column exception once implemented, and set c = NULL in that case
		c = &(t->parseColumn(col));

		if (c != NULL) {
			if (out == NULL) {
				//! Retrieve primary column
				out = &(parseTable(list, c->table)[col]);
			} else if (out->table.compare(c->table)) {  // out.table != c.table
				//! \todo Throw error: multiple primary columns with same SQLite name
			}
		}
	}

	if (out == NULL) {
		//! \todo Throw error: no such column found
	}

	return *out;
}

/*! \param list  the List of tables is which to search
 *  \param table the table to search for
 *
 *  \return a Table representing that with the given SQLite name
 */
const Database::Table &Database::Table::parseTable(const Database::Table::List &list, const std::string &table) {
	const Table *out = NULL;

	for (const Table *t : list) {
		if (!t->table.compare(table)) {  // name(t) == table
			if (out != NULL) {
				//! \todo Throw error: multiple identically-named tables
			}
			out = t;
		}
	}

	if (out == NULL) {
		//! \todo Throw error: no such table found
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
	Log << "Requesting article with id '" << id << "'..." << (Log.ENDL | Log.CONTINUE) << "    ";

	DataList rows = getColumns({}, {{ Article::columns["id"], id }});

	if (!rows.size()) {  // rows.size() == 0
		Log << "    No such article found" << Log.ENDL;
		return Article();
	} else {
		Log << "    Found" << Log.ENDL;
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
	Log << "Requesting feed with id '" << id << "'..." << (Log.ENDL | Log.CONTINUE) << "    ";

	DataList rows = getColumns({}, {{ Feed::columns["id"], id }});

	if (!rows.size()) {  // rows.size() == 0
		Log << "    No such article found" << Log.ENDL;
		return Feed();
	} else {
		Log << "    Found" << Log.ENDL;
		return Feed(rows[0]);
	}
}

/*! \return Whether the feed document differed from the saved copy\n
 *          Note that time of last update will always be changed to current time
 */
void Database::updateFeed(const std::string &id) {
	Feed local = getFeed(id);
	Feed remote(local.getURI(), local.getLang());
	//! \todo Adapt to new location and complete
	/*
	Feed remote(uri, lang);

	// id, uri should stay constant
	if (author.compare(remote.author)) {
		//! \todo Change all feeds using (previous) default author
		author = remote.author;
		changed = true;
	}
	if (link.compare(remote.description)) {
		description = remote.description;
		changed = true;
	}
	if (link.compare(remote.link)) {
		link = remote.link;
		changed = true;
	}
	if (title.compare(remote.title)) {
		//! \todo Implement and check for user-specified title
		title = remote.title;
		changed = true;
	}

	updated = time(NULL);

	// Update associated articles
	*/
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
	// If this is a new database, table is nonexistent and so contains no columns
	sqlite3_exec(db, "PRAGMA table_info(meta)", (int (*)(void*, int, char**, char**))isEmpty, &init, NULL);

	if (init) {
		//! \todo Generate programatically
		string meta =     "CREATE TABLE meta     (title,"
		                                         "version);"
		                          "INSERT INTO meta (title, version) VALUES ('" + replaceAll(filename, "'", "''") + "', 2);";
		string feeds =    "CREATE TABLE feeds    (fID        NOT NULL PRIMARY KEY ON CONFLICT REPLACE,"
		                                         "fTitle,"
		                                         "fUpdated,"
		                                         "fLink,"
		                                         "fAuthor,"
		                                         "fDesc);";
		string articles = "CREATE TABLE articles (aID       NOT NULL PRIMARY KEY ON CONFLICT REPLACE,"
		                                         "fID,"
		                                         "aTitle,"
		                                         "aUpdated,"
		                                         "aLink,"
		                                         "aAuthor,"
		                                         "aSummary,"
		                                         "aContent);";

		exec(meta.append(feeds).append(articles).c_str());
		Log << "New database initialized" << Log.ENDL;
	}
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
	DataList out;

	// If no restrictions are passed
	if (!cols.size() && !where.size()) {
		return out;
	}

	vector<string> tables;

	string cmdColumn;
	string cmdTables;
	string cmdWhere;

	// For each column requested
	bool firstCol = true;
	string t;
	for (Table::Column c : cols) {
		t = c.table;

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
		t = w.first.table;

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
	Log << "Looking for columns following command '" << cmd << "'" << Log.ENDL;
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))getEntries, &out, NULL);
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

/*! \param a the article to stage
 */
void Database::stage(const Article &a) {
	// Create a new instance so user doesn't have to worry about scope
	Article *p = new Article(a);
	Log << "Staging article '" << p->getTitle() << "'" << Log.ENDL;

	articleStage.push(p);
}

/*! \param f the feed to stage
 */
void Database::stage(const Feed &f) {
	// Create a new instance so user doesn't have to worry about scope
	Feed *p = new Feed(f);
	Log << "Staging feed '" << p->getTitle() << "'" << Log.ENDL;

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
		stage(Article(entry, *p, lang));
		++count;
	}

	Log << "Staged " << count << " articles from feed" << Log.ENDL;
}


//! \todo Update to use new Table::Column structure
void Database::save() {
	Log << "Committing staged elements" << Log.ENDL;

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

		cols = "aID, fID";
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

		Log << "Completed" << Log.ENDL;
	}

	const Feed *f;
	while (!feedStage.empty()) {
		f = feedStage.front();
		feedStage.pop();

		string id          = replaceAll(f->getID(), "'", "''");
		string title       = replaceAll(f->getTitle(), "'", "''");
		string link        = replaceAll(f->getLink(), "'", "''");
		string author      = replaceAll(f->getAuthor(), "'", "''");
		string description = replaceAll(f->getDescription(), "'", "''");

		Log << "Generating command for feed '" << title << "'...";

		cols = "fID";
		vals = "'" + id + "'";

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

		Log << "Completed" << Log.ENDL;
	}

	exec(insert);

	Log << "Saved " << countA << " articles and " << countF << " feeds from stage" << Log.ENDL;
}


/*! \param cmd SQLite3 command to execute
 */
void Database::exec(const std::string &cmd) {
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
	}

	out->push_back(m);
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
