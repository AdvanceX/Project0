//-------------------- Sqlite�������� --------------------//

typedef sqlite3      SQLITE3INST;
typedef sqlite3_stmt SQLITE3STMT;

//----------------------------------------//


//-------------------- SQLite���� --------------------//

#define sqliteOpenDb   sqlite3_open
#define sqliteShutDb   sqlite3_close
#define sqliteExecSql  sqlite3_exec
#define sqliteStepStmt sqlite3_step
#define sqlitePrepStmt sqlite3_prepare
#define sqliteFreeStmt sqlite3_finalize

//----------------------------------------//