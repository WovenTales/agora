#include <agora.hxx>

#include <article.hxx>
#include <database.hxx>
#include <feed.hxx>

int main(int argc, char *argv[]) {
	Database db(argv[2]);
	Feed f(argv[1]);
	db.stage(f);
	db.save();

	Article a = db.getArticle("http://what-if.xkcd.com/135/");
	a.print();
}


const char *agora::parseAtomTitle(const pugi::xml_node &t) {
	return t.child_value();
}

time_t agora::parseTime(const char *t) {
	return time(NULL);
}
time_t agora::parseTime(std::string t) {
	return parseTime(t.c_str());
}

std::string agora::replaceAll(std::string str, const std::string &f, const std::string &r) {
	size_t pos = 0;
	while ((pos = str.find(f, pos)) != std::string::npos) {
		str.replace(pos, f.length(), r);
		pos += r.length();
	}
	return str;
}
