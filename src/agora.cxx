#include "agora.hxx"

int main(int argc, char *argv[]) {
	Database db(argv[2]);
	Feed f(argv[1]);
	//TODO: Does not appear to correctly insert entries
	f.save(db);
}


std::string replaceAll(std::string str, const std::string &f, const std::string &r) {
	size_t pos = 0;
	while ((pos = str.find(f, pos)) != std::string::npos) {
		str.replace(pos, f.length(), r);
		pos += r.length();
	}
	return str;
}
