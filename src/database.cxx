#include "database.hxx"

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
				 "CREATE TABLE articles (aID,"
		                                        "fID,"
		                                        "aTitle,"
		                                        "aUpdated,"
		                                        "aLink,"
		                                        "aAuthor,"
		                                        "aSummary,"
		                                        "aContent);",
		             dummy, NULL, NULL);
	}
}

Database::~Database() {
	sqlite3_close(db);
}

const int Database::existed(bool *out, int cols, char *data[], char *colNames[]) {
	if (cols > 0) {
		*out = true;
	}
	return 0;
}
