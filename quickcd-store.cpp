#include <cstdlib>
#include <iostream>
using namespace std;

#include "third-party/sqlite3.h"

class Db {
 public:
	Db(const char* filename) {
		int result =
		    sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
		if (result != 0) {
			std::exit(2);
		}
	}
	~Db() {
		sqlite3_close(db);
	}
	sqlite3* db;
};

int main(int nargs, char* args[]) {
	if (nargs != 3) {
		cerr << "Usage: " << args[0] << " <dbfile> <directory to store>\n";
		return 1;
	}
	const Db db(args[1]);
	char* sql = sqlite3_mprintf(
	    "CREATE TABLE IF NOT EXISTS dirs (name text PRIMARY KEY NOT NULL, count integer NOT NULL);"
	    "BEGIN;"
	    "    INSERT INTO dirs (name, count) VALUES (\"%w\", 0) ON CONFLICT DO NOTHING;"
	    "    UPDATE dirs SET count = count + 1 WHERE name = \"%w\";"
	    "COMMIT;",
	    args[2],
	    args[2]);
	char* err;
	int result = sqlite3_exec(db.db, sql, nullptr, nullptr, &err);
	if (result != 0) {
		cerr << err << endl;
		return 3;
	}
	return 0;
}
