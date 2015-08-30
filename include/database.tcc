// Contained in namespace Database
//! \todo Document template parameters

private:

//! Obtain a Table column from a SQLite column name
/*! \todo Try to reduce redundancy
 *
 *  \param name SQLite name to parse
 */
template <typename T>
static T parseColumn(std::string name) {
	// Compare to each member of relevant column
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
	} else {
		//! \todo Throw error
	}

	//! \todo Throw error
};


//! Discard unsaved changes from a stage
/*! \param stage the stage to clear
 */
template <typename T>
void clearStaged(std::queue<T*> &stage) {
	while (!stage.empty()) {
		delete stage.front();
		stage.pop();
	}
};

//! Convert SQLite results to a usable DataList
/*! For use in sqlite3_exec() calls.
 *
 *  \todo Document params
 */
template <typename T>
static int getEntries(Database::DataList<T> *out, int cols, char *data[], char *colNames[]) {
	Data<T> m;

	for (int i = 0; i < cols; i++) {
		m[parseColumn<T>(colNames[i])] = (data[i] ? agora::replaceAll(data[i], "''", "'") : "");
	}

	out->push_back(m);
	return 0;  // Successful call
};


public:

//! Lookup specified columns in database
/*! If \c cols is an empty initializer_list, selects all columns matching \c where.
 *  Any columns in \c where that aren't in the same table as anything in \c cols are ignored.
 *
 *  \bug As it is, this might only allow a single T (i.e. only columns from a single table, rather than combining them)
 *
 *  \param cols  list of \link Database::Table Database::Table::Column\endlink objects to select
 *  \param where pairs of (\link Database::Table Database::Table::Column\endlink, match_string); optional unless \c cols is empty
 */
template <typename T>
DataList<T> getColumns(const std::initializer_list<Table::Unified<T> > &cols,
                       const std::initializer_list<std::pair<Table::Unified<T>, std::string> > &where = {}) const {
	// If no restrictions are passed
	if (!cols.size() && !where.size()) {
		//! \todo Throw an error as we can't tell even what table is wanted
	}

	std::vector<std::string> tables;
	std::string cmdColumn;
	std::string cmdTables;
	std::string cmdWhere;

	// For each column requested
	bool firstCol = true;
	for (Table::Unified<T> c : cols) {
		// If c's containing table is not already in tables
		if (find(tables.begin(), tables.end(), Table::table(c)) == tables.end()) {
			tables.push_back(Table::table(c));
		}

		// Add column to command
		if (firstCol) {
			cmdColumn = Table::column(c);
			firstCol = false;
		} else {
			cmdColumn.append("," + Table::column(c));
		}
	}

	// For each search restriction
	firstCol = true;
	for (std::pair<const Table::Unified<T>, std::string> w : where) {
		// If no columns are requested OR w's containing table is in tables
		// In other words, ignores irrelevant restrictions
		if (!cols.size() || (find(tables.begin(), tables.end(), Table::table(w.first)) != tables.end())) {
			// If there are no requested columns AND w's containing table is not in tables
			if (!cols.size() && (find(tables.begin(), tables.end(), Table::table(w.first)) == tables.end())) {
				tables.push_back(Table::table(w.first));
			}

			// Add restriction to command
			if (firstCol) {
				cmdWhere = " WHERE ";
				firstCol = false;
			} else {
				cmdWhere.append(" AND ");
			}
			cmdWhere.append(Table::column(w.first) + " = '" + agora::replaceAll(w.second, "'", "''") + "'");
		}
	}

	// Join all involved tables
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

	// Execute the compiled command
	Log << "Looking for columns following command '" << cmd << "'" << Log.ENDL;
	DataList<T> out;
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))getEntries<T>, &out, NULL);
	return out;
};
