#include <agora.hxx>

#include <article.hxx>
#include <database.hxx>
#include <feed.hxx>

#include <curl/curl.h>
#include <locale.h>
#include <sstream>
#include <typeinfo>


int main(int argc, char *argv[]) {
	// Enable Unicode glyphs (assuming UTF-8 locale + article encoding)
	setlocale(LC_CTYPE, "");
	//! \todo Call curl_global_init() with proper params

	Database db(argv[1]);
	db.stage(Feed("file://bigfinish.rss"));
	db.stage(Feed("file://whatif.atom"));
	db.save();

	Article a = db.getArticle("http://what-if.xkcd.com/135/");
	a.print();

	// Cleanup
	curl_global_cleanup();
}



//! Callback required for cURL calls
/*! \param ptr   data obtained by cURL
 *  \param size  one dimension of data size
 *  \param nmemb one dimension of data size
 *  \param dest  std::string* to write into
 */
size_t curlWrite(char *ptr, size_t size, size_t nmemb, void *dest) {
	for (unsigned int i = 0; i < (size * nmemb); ++i) {
		((std::string*) dest)->push_back(ptr[i]);
	}

	return (size * nmemb);  // tell curl how many bytes we handled
}
/*! \param url the page to download
 *  \return A string representation of the file
 */
std::string agora::download(const std::string &url) {
	Log << "Downloading file from '" << url << "'...";
	std::string data;

	CURL *curl = curl_easy_init();
	if (curl == NULL) {
		//! \todo Throw error
	}

	//! \todo Check for and handle error at each step according to libcurl-errors (want CURLE_OK)
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

	if (curl_easy_perform(curl) != CURLE_OK) {
		//! \todo Throw error
	}

	curl_easy_cleanup(curl);

	Log << "Completed" << Log.ENDL;
	return data;
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

/*! Wrapper for curl_getdate() to simplify #includes.
 *
 *  \param t the string to parse
 *  \return Correctly parsed time
 */
time_t agora::parseTime(const std::string &t) {
	return curl_getdate(t.c_str(), NULL);
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
