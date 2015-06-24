#include <database.hxx>

#include <agora.hxx>
#include <article.hxx>
#include <feed.hxx>
#include <logger.hxx>

#include <algorithm>
#include <iostream>
#include <string.h>

using namespace agora;
using namespace pugi;
using namespace std;


// Note that every title must be unique!
Database::Column::Name Database::Column::DBTitle(         "meta",    "title");
Database::Column::Name Database::Column::DBVersion(       "meta",    "version");

Database::Column::Name Database::Column::ArticleID(       "articles", "aID");
Database::Column::Name Database::Column::ArticleTitle(    "articles", "aTitle");
Database::Column::Name Database::Column::ArticleUpdated(  "articles", "aUpdated");
Database::Column::Name Database::Column::ArticleLink(     "articles", "aLink");
Database::Column::Name Database::Column::ArticleAuthor(   "articles", "aAuthor");
Database::Column::Name Database::Column::ArticleSummary(  "articles", "aSummary");
Database::Column::Name Database::Column::ArticleContent(  "articles", "aContent");

Database::Column::Name Database::Column::FeedID(          "feeds",    "fID");
Database::Column::Name Database::Column::FeedTitle(       "feeds",    "fTitle");
Database::Column::Name Database::Column::FeedLink(        "feeds",    "fLink");
Database::Column::Name Database::Column::FeedAuthor(      "feeds",    "fAuthor");
Database::Column::Name Database::Column::FeedDescription( "feeds",    "fDesc");


Database::Column::Name Database::Column::parse(const std::string &name) {
	static vector<Name> cols = {
		DBTitle,
		DBVersion,

		ArticleID,
		ArticleTitle,
		ArticleUpdated,
		ArticleLink,
		ArticleAuthor,
		ArticleSummary,
		ArticleContent,

		FeedID,
		FeedTitle,
		FeedLink,
		FeedAuthor,
		FeedDescription
	};

	for (Name c : cols) {
		if (c.column == name) {
			return c;
		}
	}

	//! \todo Throw error as incorrect column title.
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
	--(*this);
}


Database &Database::operator=(const Database &d) {
	count = d.count;
	db    = d.db;

	++count;
}

Database &Database::operator--() {
	if (--count == 0) {
		close();

		delete &count;
	}
}

//! \todo Has slight inneffeciency as if \c where is an id, can unnecessarily join table (eg. SELECT * FROM articles WHERE fID = '...').
/*! If \c cols is an empty initializer_list, selects all columns matching \c where.
 *
 *  \param where pairs of (Database::Column::Name, match_string); optional unless \c cols is empty
 */
Database::DataList Database::getColumns(const std::initializer_list<Database::Column::Name> &cols, const std::initializer_list<std::pair<Database::Column::Name, std::string> > &where) const {
	if (!cols.size() && !where.size()) {
		//! \todo Throw an error.
	}

	vector<string> tables;
	string cmdColumn;
	string cmdTables;
	string cmdWhere;

	bool firstCol = true;
	for (Column::Name c : cols) {
		if (find(tables.begin(), tables.end(), c.table) == tables.end()) {
			tables.push_back(c.table);
		}

		if (firstCol) {
			cmdColumn = c.column;
			firstCol = false;
		} else {
			cmdColumn.append("," + c.column);
		}
	}

	firstCol = true;
	for (pair<const Column::Name, string > w : where) {
		if (find(tables.begin(), tables.end(), w.first.table) == tables.end()) {
			tables.push_back(w.first.table);
		}

		if (firstCol) {
			cmdWhere = " WHERE ";
			firstCol = false;
		} else {
			cmdWhere.append(" AND ");
		}
		cmdWhere.append(w.first.column + " = '" + replaceAll(w.second, "'", "''") + "'");
	}

	firstCol = true;
	for (string t : tables) {
		if (firstCol) {
			cmdTables = t;
			firstCol = false;
		} else {
			cmdTables.append(" NATURAL JOIN " + t);
		}
	}

	string cmd = "SELECT " + (cols.size() ? cmdColumn : "*") + " FROM " + cmdTables + cmdWhere + ";";

	Log << "Looking for columns following command '" << cmd << "'" << Log.ENDL;
	DataList out;
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))getEntries, &out, NULL);
	return out;
}

std::string Database::getTitle() const {
	DataList out = getColumns({ Column::DBTitle });
	return replaceAll(out[0][Column::DBTitle], "''", "'");
}

/*! If database doesn't contain given ID, returns an Article with default values
 *  (mostly empty strings) for all members.
 *
 *  \param id article ID
 *  \return An Article with values according to given ID
 */
Article Database::getArticle(const std::string &id) const {
	Log << "Requesting article with id '" << id << "'..." << (Log.ENDL | Log.CONTINUE);

	DataList rows = getColumns({}, {{ Column::ArticleID, id }});

	if (!rows.size()) {  // rows.size() == 0
		Log << Log.CONTINUE << "No such article found" << Log.ENDL;
		return Article();
	} else {
		Log << Log.CONTINUE << "Found" << Log.ENDL;
		return Article(makeArticle(rows[0]));
	}
}

/*! \param data the Data to parse
 *  \return The resulting Article
 */
Article Database::makeArticle(const Database::Data &data) {
	string id, feedID, title, author, content, link, summary;
	time_t updated;

	for (pair<const Column::Name, string > c : data) {
		if (!c.second.empty()) {
			     if (c.first == Column::ArticleID)      id      = c.second;
			else if (c.first == Column::FeedID)         feedID  = c.second;
			else if (c.first == Column::ArticleTitle)   title   = c.second;
			else if (c.first == Column::ArticleUpdated) updated = parseTime(c.second);
			else if (c.first == Column::ArticleLink)    link    = c.second;
			else if (c.first == Column::ArticleAuthor)  author  = c.second;
			else if (c.first == Column::ArticleSummary) summary = c.second;
			else if (c.first == Column::ArticleContent) content = c.second;
		}
	}

	return Article(id, feedID, title, link, updated, author, content, summary);
}


/*! If \c force set to true, avoids saving staged changes before closing database.
 *
 *  \param force whether to skip saving changes (default: save)
 */
void Database::close(bool force) {
	if (db) {
		Log << "Closing database" << Log.ENDL;

		if (force) {
			clearStaged();
		} else {
			save();
		}
		sqlite3_close(db);

		db = NULL;
	}
}


/*! If this already has an active database, saves changes to that and closes it rather than overwriting.
 *
 *  \param filename the feed database to associate this Database with
 */
void Database::open(const std::string &filename) {
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
	sqlite3_exec(db, "PRAGMA table_info(feeds)", (int (*)(void*, int, char**, char**))isEmpty, &init, NULL);

	if (init) {
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

	string id = p->getID();
	FeedLang lang = p->getLang();
	xml_node root = p->getRoot();
	const char *tag = (lang == ATOM ? "entry" : "item");
	unsigned int count = 0;

	for (xml_node entry = root.child(tag);
	     entry.type(); // != NULL
	     entry = entry.next_sibling(tag)) {
		stage(Article(entry, *p, lang));
		count++;
	}

	Log << "Staged " << count << " articles from feed" << Log.ENDL;
}


void Database::save() {
	Log << "Committing staged elements" << Log.ENDL;

	unsigned int countF = 0, countA = 0;
	string insert("");
	string cols("");
	string vals("");

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

int Database::getEntries(Database::DataList *out, int cols, char *data[], char *colNames[]) {
	Database::Data m;

	for (int i = 0; i < cols; i++) {
		m[Column::parse(colNames[i])] = (data[i] ? replaceAll(data[i], "''", "'") : "");
	}

	out->push_back(m);
	return 0;
}

int Database::isEmpty(bool *out, int cols, char *data[], char *colNames[]) {
	if (cols == 0) {
		*out = true;
	} else {
		*out = false;
	}
	return 0;
}
