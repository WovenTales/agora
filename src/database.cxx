#include <database.hxx>

#include <article.hxx>

#include <iostream>
#include <string.h>

Database::Database(const char *filename) {
	if (sqlite3_open(filename, &db) != SQLITE_OK) {
		//TODO: More robust error handling
		cout << "Bad database " << filename << endl;
	}

	bool old = false;
	sqlite3_exec(db, "PRAGMA table_info(feeds)", (int (*)(void*, int, char**, char**))existed, &old, NULL);

	if (!old) {
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
	}
}

Database::~Database() {
	sqlite3_close(db);
}

Article Database::getArticle(string id) {
	Article out;
	string cmd("SELECT * FROM articles where aID = '" + replaceAll(id, "'", "''") + "';");
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))makeArticle, &out, NULL);
	return out;
}

const int Database::existed(bool *out, int cols, char *data[], char *colNames[]) {
	if (cols > 0) {
		*out = true;
	}
	return 0;
}

int Database::makeArticle(Article *a, int cols, char *vals[], char *names[]) {
	char *curName, *curVal;
	string id, feedID, title, author, content, link, summary;
	time_t updated;

	for (int i = 0; i < cols; i++) {
		curName = names[i];
		curVal = vals[i];

		if (curVal && !strcmp(curName, "aID")) {  // curVal != NULL && curName == "aID"
			id = curVal;
		} else if (curVal && !strcmp(curName, "fID")) {  // curVal != NULL && curName == "fID"
			feedID = string(curVal);
		} else if (curVal && !strcmp(curName, "aTitle")) {  // curVal != NULL && curName == "aTitle"
			title = string(curVal);
		} else if (curVal && !strcmp(curName, "aUpdated")) {  // curVal != NULL && curName == "aUpdated"
			updated = parseTime(string(curVal));
		} else if (curVal && !strcmp(curName, "aLink")) {  // curVal != NULL && curName == "aLink"
			link = string(curVal);
		} else if (curVal && !strcmp(curName, "aAuthor")) {  // curVal != NULL && curName == "aAuthor"
			author = string(curVal);
		} else if (curVal && !strcmp(curName, "aSummary")) {  // curVal != NULL && curName == "aSummary"
			summary = string(curVal);
		} else if (curVal && !strcmp(curName, "aContent")) {  // curVal != NULL && curName == "aContent"
			content = string(curVal);
		}
	}

	*a = Article(id, feedID, title, updated, author, content, link, summary);

	return 0;
}


time_t Database::parseTime(const char *t) {
	return time(NULL);
}
time_t Database::parseTime(string t) {
	return parseTime(t.c_str());
}
