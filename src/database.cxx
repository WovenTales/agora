#include <database.hxx>

#include <agora.hxx>
#include <article.hxx>
#include <feed.hxx>
#include <logger.hxx>

#include <iostream>
#include <string.h>

using namespace agora;
using namespace pugi;
using namespace std;


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


std::string Database::getTitle() {
	vector<Data> *rows = exec("SELECT title FROM meta", 0);
	string out = "";

	if (rows->size()) {
		out = replaceAll((*rows)[0]["title"], "''", "'");
	}

	delete rows;
	return out;
}

/*! If database doesn't contain given ID, returns an Article with default values
 *  (mostly empty strings) for all members.
 *
 *  \param id article ID
 *  \return An Article with values according to given ID
 */
Article Database::getArticle(const std::string &id) {
	Log << "Requesting article with id '" << id << "'...";

	string cmd("SELECT * FROM articles WHERE aID = '" + replaceAll(id, "'", "''") + "';");
	vector<Data> *rows = exec(cmd, 0);

	if (!rows->size()) {  // rows.size() == 0
		Log << "No such article found" << Log.ENDL;
		delete rows;
		return Article();
	} else {
		Log << "Found" << Log.ENDL;
		Data data = (*rows)[0];
		delete rows;
		return Article(makeArticle(data));
	}
}

/*! \param data the Data to parse
 *  \return The resulting Article
 */
Article Database::makeArticle(const Database::Data data) {
	string id, feedID, title, author, content, link, summary;
	time_t updated;

	string curName, curVal;
	Data::const_iterator end = data.end();
	for (Data::const_iterator it = data.begin(); it != end; it++) {
		curName = it->first;
		curVal = it->second;

		if (!curVal.empty() && !curName.compare("aID")) {  // curVal != "" && curName == "aID"
			id = curVal;
		} else if (!curVal.empty() && !curName.compare("fID")) {  // curVal != "" && curName == "fID"
			feedID = curVal;
		} else if (!curVal.empty() && !curName.compare("aTitle")) {  // curVal != "" && curName == "aTitle"
			title = curVal;
		} else if (!curVal.empty() && !curName.compare("aUpdated")) {  // curVal != "" && curName == "aUpdated"
			updated = parseTime(curVal);
		} else if (!curVal.empty() && !curName.compare("aLink")) {  // curVal != "" && curName == "aLink"
			link = curVal;
		} else if (!curVal.empty() && !curName.compare("aAuthor")) {  // curVal != "" && curName == "aAuthor"
			author = curVal;
		} else if (!curVal.empty() && !curName.compare("aSummary")) {  // curVal != "" && curName == "aSummary"
			summary = curVal;
		} else if (!curVal.empty() && !curName.compare("aContent")) {  // curVal != "" && curName == "aContent"
			content = curVal;
		}
	}

	return Article(id, feedID, title, link, updated, author, content, summary);
}


/*! If \c force set to true, avoids saving staged changes before closing database.
 *
 *  \param force whether to skip saving changes (default: save)
 */
void Database::close(bool force) {
	Log << "Closing database" << Log.ENDL;

	if (force) {
		clearStaged();
	} else {
		save();
	}
	sqlite3_close(db);

	db = NULL;
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
		sqlite3_exec(db, meta.append(feeds).append(articles).c_str(), NULL, NULL, NULL);
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
			//! \bug Trying to insert (or later update with) content results in command being ignored\n
			//! When updated manually, get/makeArticle properly displays, so is likely buffer error of some sort
			//cols += ", aContent";
			//vals += ",'" + content + "'";
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
/*! The user is expected to \c delete the returned \c vector when done.
 *
 *  \param cmd SQLite3 command to execute
 *  \param i   dummy parameter to differentiate from Database::exec(const std::string&)
 *  \return List of Data generated from \p cmd
 */
std::vector<Database::Data> *Database::exec(const std::string &cmd, int i) {
	vector<Database::Data> *out = new vector<Database::Data>;

	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))getEntries, out, NULL);

	return out;
}

int Database::getEntries(std::vector<Database::Data> *out, int cols, char *data[], char *colNames[]) {
	Database::Data m;

	for (int i = 0; i < cols; i++) {
		m[colNames[i]] = (data[i] ? replaceAll(data[i], "''", "'") : "");
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
