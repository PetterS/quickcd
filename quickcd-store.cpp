#include <cstdlib>
#include <cstring>
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
		sqlite3_busy_timeout(db, 10000);
	}
	~Db() {
		sqlite3_close(db);
	}
	sqlite3* db;
};

int main(int nargs, char* args[]) {
	if (nargs != 3 && nargs != 6) {
		cerr << "Usage: " << args[0] << " <dbfile> <directory to store>\n";
		cerr << "Usage: " << args[0]
		     << " <dbfile> <directory to store> <command> <return val> <time>\n";
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
	char* err = nullptr;
	int result = sqlite3_exec(db.db, sql, nullptr, nullptr, &err);
	sqlite3_free(sql);

	if (result != 0) {
		if (err != nullptr) {
			cerr << err << endl;
			sqlite3_free(err);
		}
		return 3;
	}

	if (nargs == 6) {
		char* sql = sqlite3_mprintf(
		    "CREATE TABLE IF NOT EXISTS commands (timestamp datetime default current_timestamp,"
		    "                                     command text,"
		    "                                     directory text,"
		    "                                     return_value integer,"
		    "                                     time real"
		    ");"
		    "BEGIN;"
		    "INSERT INTO commands (command, directory, return_value, time) VALUES (\"%w\", \"%w\", "
		    "\"%w\", \"%w\");"
		    "COMMIT;",
		    args[3],
		    args[2],
		    args[4],
		    args[5]);
		char* err = nullptr;
		int result = sqlite3_exec(db.db, sql, nullptr, nullptr, &err);
		sqlite3_free(sql);

		if (result != 0) {
			if (err != nullptr) {
				cerr << err << endl;
				sqlite3_free(err);
			}
			return 3;
		}
	}

	return 0;
}
