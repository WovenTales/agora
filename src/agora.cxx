#include <agora.hxx>

#include <ncursesui.hxx>


int main(int argc, char *argv[]) {
	NcursesUI *ui = new NcursesUI();
	delete ui;
}



/*! \todo See http://atomenabled.org/developers/syndication/#text
 *
 *  \param t the Atom node to parse
 *  \return A string representation of the given title
 */
std::string agora::parseAtomTitle(const pugi::xml_node &t) {
	std::string title = t.child_value();
	return title;
}

/*! \todo Actually parse the time.\n
 *          Might be an already existing library to simplify this; check for one
 *  \todo Will likely require some ability to disambiguate eg. `04-02-10` based on patten in other articles from same feed
 *
 *  \param t the string to parse
 *  \return Correctly parsed time
 */
time_t agora::parseTime(const std::string &t) {
	return time(NULL);
}

/*! Does not overwrite any already-replaced substring:
 *  if the replacement is `ab` -> `a`, the sequence `abb` in the original
 *  string will result in `ab` in the output, not `a`.
 *
 *  \param str the original string
 *  \param f   the substring to be replaced
 *  \param r   the replacement for `f`
 *  \return str after all instances of `f` have been replaced with `r`
 */
std::string agora::replaceAll(std::string str, const std::string &f, const std::string &r) {
	size_t pos = 0;
	while ((pos = str.find(f, pos)) != std::string::npos) {
		str.replace(pos, f.length(), r);
		// Incrementing pos allows for avoiding overwriting parts of r on successive iterations
		pos += r.length();
	}
	return str;
}
