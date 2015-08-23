#include <agora.hxx>

//#include <database.hxx>
//#include <feed.hxx>
#include <ui/ncurses/ui.hxx>

#include <locale.h>
#include <sstream>


int main(int argc, char *argv[]) {
	/*
	Database db(argv[1]);
	db.stage(Feed("bigfinish.rss"));
	db.stage(Feed("whatif.atom"));
	db.save();
	*/

	// Enable Unicode glyphs (assuming UTF-8 locale + article encoding)
	setlocale(LC_CTYPE, "");

	NcursesUI ui(argv[1]);
}



/*! \todo Implement: see http://atomenabled.org/developers/syndication/#text
 *
 *  \param t the Atom node to parse
 *  \return A string representation of the given title
 */
std::string agora::parseAtomTitle(const pugi::xml_node &t) {
	std::string title = t.child_value();
	return title;
}

/*! \todo Implement; might be an already existing library to simplify this
 *        Will likely require some ability to disambiguate eg. `04-02-10` based on patten in other articles from same feed
 *
 *  \param t the string to parse
 *  \return Correctly parsed time
 */
time_t agora::parseTime(const std::string &t) {
	return time(NULL);
}


/*! Does not overwrite any already-replaced substring:
 *  if the replacement is `ab` → `a`, the sequence `abb` in the original
 *  string will output `ab`, not `a`.
 *
 *  \param str the original string
 *  \param f   the substring to be replaced
 *  \param r   the replacement for `f`
 *  \return `str` after all instances of `f` have been replaced with `r`
 */
std::string agora::replaceAll(std::string str, const std::string &f, const std::string &r) {
	size_t pos = 0;
	// Iterate over length of string
	while ((pos = str.find(f, pos)) != std::string::npos) {
		str.replace(pos, f.length(), r);
		// Incrementing pos avoids overwriting parts of r on successive iterations
		pos += r.length();
	}
	return str;
}

/*! Delimiter behavior follows that of std::getline().
 *
 *  \param in the original text block
 *  \param d  the character on which to split
 *  \return The resulting strings encapsulated in a std::vector
 */
std::vector<std::string> agora::splitString(const std::string &in, char d) {
	std::vector<std::string> out;

	std::istringstream s(in);
	std::string line;
	// Repeat until end of string
	while (std::getline(s, line, d)) {
		if (!line.empty()) {
			out.push_back(line);
		}
	}

	return out;
}
/*! Delimiter behavior follows that of std::getline().
 *
 *  \param in the original text
 *  \param d  the character on which to split
 *  \return The resulting strings encapsulated in a new std::vector
 */
std::vector<std::string> agora::splitString(const std::vector<std::string> &in, char d) {
	std::vector<std::string> out;

	std::istringstream s;
	for (std::string line : in) {
		// Repeat until end of string
		while (std::getline(s, line, d)) {
			if (!line.empty()) {
				out.push_back(line);
			}
		}
	}

	return out;
}
