// Contained in namespace Database

private:

template <typename T>
static T parseColumn(std::string name) {
	if (typeid(T) == typeid(Table::Article)) {
		for (auto c : ArticleColumnName) {
			if (!name.compare(Table::column(c.first))) {  // name == column(c)
				return (T) c.first;
			}
		}
	} else if (typeid(T) == typeid(Table::Feed)) {
		for (auto c : FeedColumnName) {
			if (!name.compare(Table::column(c.first))) {  // name == column(c)
				return (T) c.first;
			}
		}
	} else if (typeid(T) == typeid(Table::Meta)) {
		for (auto c : MetaColumnName) {
			if (!name.compare(Table::column(c.first))) {  // name == column(c)
				return (T) c.first;
			}
		}
	}

	//! \todo Throw error.
};


template <typename T>
void clearStaged(std::queue<T*> &stage) {
	while (!stage.empty()) {
		delete stage.front();
		stage.pop();
	}
};

// For use in sqlite3_exec() calls
template <typename T>
static int getEntries(Database::DataList<T> *out, int cols, char *data[], char *colNames[]) {
	Data<T> m;

	for (int i = 0; i < cols; i++) {
		m[parseColumn<T>(colNames[i])] = (data[i] ? agora::replaceAll(data[i], "''", "'") : "");
	}

	out->push_back(m);
	return 0;
};


public:

//! Lookup specifed columns in database.
/*! If \c cols is an empty initializer_list, selects all columns matching \c where.
 *  Any columns in \c where that aren't in the same table as anything in \c cols are ignored.
 *
 *  \todo As it is, this only allows a single T (ie. only columns from a single table, rather than combining them).
 *
 *  \param where pairs of (Database::Table::TableName::Column, match_string); optional unless \c cols is empty
 */
template <typename T>
DataList<T> getColumns(const std::initializer_list<Table::Unified<T> > &cols,
                       const std::initializer_list<std::pair<Table::Unified<T>, std::string> > &where = {}) const {
	if (!cols.size() && !where.size()) {
		//! \todo Throw an error.
	}

	std::vector<std::string> tables;
	std::string cmdColumn;
	std::string cmdTables;
	std::string cmdWhere;

	bool firstCol = true;
	for (Table::Unified<T> c : cols) {
		if (find(tables.begin(), tables.end(), Table::table(c)) == tables.end()) {
			tables.push_back(Table::table(c));
		}

		if (firstCol) {
			cmdColumn = Table::column(c);
			firstCol = false;
		} else {
			cmdColumn.append("," + Table::column(c));
		}
	}

	firstCol = true;
	for (std::pair<const Table::Unified<T>, std::string> w : where) {
		if (!cols.size() || (find(tables.begin(), tables.end(), Table::table(w.first)) != tables.end())) {
			if (!cols.size() && (find(tables.begin(), tables.end(), Table::table(w.first)) == tables.end())) {
				tables.push_back(Table::table(w.first));
			}

			if (firstCol) {
				cmdWhere = " WHERE ";
				firstCol = false;
			} else {
				cmdWhere.append(" AND ");
			}
			cmdWhere.append(Table::column(w.first) + " = '" + agora::replaceAll(w.second, "'", "''") + "'");
		}
	}

	firstCol = true;
	for (std::string t : tables) {
		if (firstCol) {
			cmdTables = t;
			firstCol = false;
		} else {
			cmdTables.append(" NATURAL JOIN " + t);
		}
	}

	std::string cmd = "SELECT " + (cols.size() ? cmdColumn : "*") + " FROM " + cmdTables + cmdWhere + ";";

	Log << "Looking for columns following command '" << cmd << "'" << Log.ENDL;
	DataList<T> out;
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))getEntries<T>, &out, NULL);
	return out;
};
