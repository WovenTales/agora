#include "agora.hxx"

int main(int argc, char *argv[]) {
	Database db(argv[2]);
	Feed f(argv[1]);
	//TODO: Does not appear to correctly insert entries
	f.save(db);
}
