#include "database.hxx"

Database::Database() {;}
Database::Database(const char *filename) {
	sqlite3_open(filename, &db);
}

Database::~Database() {
	sqlite3_close(db);
}
