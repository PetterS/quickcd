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

	void run(const char* sql,
	         int (*callback)(void*, int, char**, char**) = nullptr,
	         void* data = nullptr) {
		char* err = nullptr;
		int result = sqlite3_exec(db, sql, callback, data, &err);
		if (result != 0) {
			if (err != nullptr) {
				cerr << err << endl;
				sqlite3_free(err);
			}
			cerr << "FAIL: " << sql << "\n";
			std::exit(2);
		}
	}

	sqlite3* db;
};

static int set_int(void* data, int, char** result, char**) {
	int* number = (int*)data;
	*number = std::atoi(result[0]);
	return 0;
}

static void migrate(Db* db) {
	int user_version = 0;
	db->run("pragma user_version;", set_int, &user_version);
	switch (user_version) {
		default:
			cerr << "Unexpected user_version!\n";
			std::exit(3);

		case 0:
			db->run(
			    "CREATE TABLE IF NOT EXISTS dirs (name text PRIMARY KEY NOT NULL, count integer NOT "
			    "NULL);\n"
			    "CREATE TABLE IF NOT EXISTS commands (timestamp datetime default current_timestamp,"
			    "                                     command text,"
			    "                                     directory text,"
			    "                                     return_value integer,"
			    "                                     time real"
			    ");\n"
			    "PRAGMA journal_mode=WAL;\n"
			    "PRAGMA user_version = 1;");
			cerr << "DB migrated to version 1\n";
		case 1:
		    /*current version*/;
	}
	db->run("pragma user_version;", set_int, &user_version);
}

int main(int nargs, char* args[]) {
	if (nargs != 3 && nargs != 6) {
		cerr << "Usage: " << args[0] << " <dbfile> <directory to store>\n";
		cerr << "Usage: " << args[0]
		     << " <dbfile> <directory to store> <command> <return val> <time>\n";
		return 1;
	}
	Db db(args[1]);
	migrate(&db);

	char* sql = sqlite3_mprintf(
	    "BEGIN;"
	    "    INSERT INTO dirs (name, count) VALUES (%Q, 0) ON CONFLICT DO NOTHING;"
	    "    UPDATE dirs SET count = count + 1 WHERE name = %Q;"
	    "COMMIT;",
	    args[2],
	    args[2]);
	db.run(sql);
	sqlite3_free(sql);

	if (nargs == 6) {
		char* sql = sqlite3_mprintf(
		    "BEGIN;"
		    "INSERT INTO commands (command, directory, return_value, time) VALUES (%Q, %Q, "
		    "%Q, %Q);"
		    "COMMIT;",
		    args[3],
		    args[2],
		    args[4],
		    args[5]);
		db.run(sql);
		sqlite3_free(sql);
	}

	return 0;
}
