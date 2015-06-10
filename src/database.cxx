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


/*! \param filename the SQLite3 database to associate this Database with
 */
Database::Database(const std::string &filename) {
	Logger::log("Opening database '" + filename + "'...", Logger::CONTINUE);

	if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
		//! \todo More robust error handling
		cout << "Bad database " << filename << endl;
	}

	Logger::log("Completed");

	bool init = true;
	sqlite3_exec(db, "PRAGMA table_info(feeds)", (int (*)(void*, int, char**, char**))isEmpty, &init, NULL);

	if (init) {
		// Initialize database
		sqlite3_exec(db, "CREATE TABLE feeds (fID        NOT NULL PRIMARY KEY ON CONFLICT REPLACE,"
		                                      "fTitle,"
		                                      "fUpdated,"
		                                      "fLink,"
		                                      "fAuthor,"
		                                      "fDesc);"
				 "CREATE TABLE articles (aID       NOT NULL PRIMARY KEY ON CONFLICT REPLACE,"
		                                        "fID,"
		                                        "aTitle,"
		                                        "aUpdated,"
		                                        "aLink,"
		                                        "aAuthor,"
		                                        "aSummary,"
		                                        "aContent);",
		             NULL, NULL, NULL);
		Logger::log("New database initialized");
	}
}

Database::~Database() {
	save();
	sqlite3_close(db);
}


/*! If database doesn't contain given ID, returns an Article with default values
 *  (mostly empty strings) for all members.
 *
 *  The returned article should be freed with \c delete when done.
 *
 *  \param id article ID
 *  \return Pointer to a new Article with values according to given ID
 */
Article *Database::getArticle(const std::string &id) {
	Logger::log("Requesting article with id '" + id + "'...", Logger::CONTINUE);

	Article *out = NULL;
	string cmd("SELECT * FROM articles WHERE aID = '" + replaceAll(id, "'", "''") + "';");
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))makeArticle, &out, NULL);

	if (!out) {
		Logger::log("No such article found");
		out = new Article();
	} else {
		Logger::log("Found");
	}

	return out;
}


/*! \param a the article to stage
 */
void Database::stage(const Article &a) {
	// Create a new instance so user doesn't have to worry about scope
	Article *p = new Article(a);
	Logger::log("Staging article '" + p->getTitle() + "'");

	articleStage.push(p);
}
/*! \param f the feed to stage
 */

void Database::stage(const Feed &f) {
	// Create a new instance so user doesn't have to worry about scope
	Feed *p = new Feed(f);
	Logger::log("Staging feed '" + p->getTitle() + "'");

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

	Logger::log("Staged ", Logger::CONTINUE);
	Logger::log(count, Logger::CONTINUE);
	Logger::log(" articles from feed");
}


void Database::save() {
	Logger::log("Committing staged elements");

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

		Logger::log("Generating command for article '" + title + "'...", Logger::CONTINUE);

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
			cols += ", aContent";
			//vals += ",'" + content + "'";
			vals += ",'SKIPPED'";
		}

		//! \todo Only update what's necessary rather than replacing everything
		insert += "INSERT INTO articles (" + cols + ") VALUES (" + vals + ");";

		delete a;
		countA++;

		Logger::log("Completed");
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

		Logger::log("Generating command for feed '" + title + "'...", Logger::CONTINUE);

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

		Logger::log("Completed");
	}

	exec(insert);

	Logger::log("Saved ", Logger::CONTINUE);
	Logger::log(countA, Logger::CONTINUE);
	Logger::log(" articles and ", Logger::CONTINUE);
	Logger::log(countF, Logger::CONTINUE);
	Logger::log(" feeds from stage");
}


int Database::isEmpty(bool *out, int cols, char *data[], char *colNames[]) {
	if (cols == 0) {
		*out = true;
	} else {
		*out = false;
	}
	return 0;
}

int Database::makeArticle(Article **a, int cols, char *vals[], char *names[]) {
	//! \todo Delete old article at *a

	string curName, curVal;
	string id, feedID, title, author, content, link, summary;
	time_t updated;

	for (int i = 0; i < cols; i++) {
		curName = names[i];
		curVal = (vals[i] ? replaceAll(vals[i], "''", "'") : "");

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

	*a = new Article(id, feedID, title, link, updated, author, content, summary);

	return 0;
}
