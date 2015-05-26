#include "agora.hxx"

int main(int argc, char *argv[]) {
	Feed f(argv[1]);
	Database db;
	f.print();
	f.save(db);
}
