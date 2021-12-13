/*
多个open对一个文件，肯定涉及file lock的情况。insert可能会返回SQLITE_LOCKED

不开WAL
一个open足矣，线程模型SQLITE_CONFIG_SERIALIZED(默认)，库底层保证其线程安全，锁粒度更细。写用缓冲 + 事务

开WAL
设置合适的WAL SIZE
一个open 写，线程模型SQLITE_CONFIG_SERIALIZED
多个open 读，线程模型SQLITE_CONFIG_SERIALIZED
都要考虑file locked的情况

进程异常断掉，会留下journal文件，不要删，下次启动的时候回加载并自动删除


优化：
（1）一个open足矣，默认线程模型，一个线程写，多个线程读
（2）写要用缓冲+事务
（3）多open可以考虑WAL，但是要注意SQLITE_LOCKED

*/

#include <sqlite3.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/time.h>

using namespace std;

static const std::string create_db_sql = " \
                    create table if not exists `my_table` ( \
                        `id` INTEGER primary key autoincrement, \
                        `aaa` INTEGER not null default 0, \
                        `time` DATETIME not null DEFAULT CURRENT_TIMESTAMP \
                    ); \
                    create index if not exists idx_aaa on my_table(`aaa`); \
                    ";

void InsertFunc(sqlite3* db) {
    time_t t1;
    ::time(&t1);

    sqlite3_stmt* stmt = nullptr;
    std::string sql = "insert into my_table (aaa) values (?)";
    int ret = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (ret != SQLITE_OK) {
        cout << "prepare err " << endl;
        return;
    }
    // ret = sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    // if (ret != SQLITE_OK) {
    //     cout << "begin tran error " << endl;
    //     return;
    // }
    for (size_t i = 0; i < 10000; i++) {
        sqlite3_bind_int(stmt, 1, i);
        int ret = sqlite3_step(stmt);
        if (ret != SQLITE_DONE) {
            cout << i << " insert error " << ret << endl;
        }
        sqlite3_reset(stmt);
    }

    // sqlite3_exec(db, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    sqlite3_finalize(stmt);

    time_t t2;
    ::time(&t2);
    cout << t2 - t1 << endl;
}

void QueryFunc(sqlite3* db) {
    sqlite3_stmt* stmt = nullptr;
    std::string sql = "select count(*) from my_table where aaa > 2000";
    int ret = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (ret != SQLITE_OK) {
        cout << "prepare err " << endl;
        return;
    }
    while(1) {
        int ret = sqlite3_step(stmt);
        switch (ret) {
            case SQLITE_ROW:
                // cout << std::this_thread::get_id() << " count " << sqlite3_column_int(stmt, 0) << endl;
                break;
            case SQLITE_DONE:
                break;
            default:
                cout << "query error " << ret << endl;
        }
        sqlite3_reset(stmt);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    sqlite3_finalize(stmt);
}

sqlite3* NewConn() {
    sqlite3* db;
    int ret = sqlite3_open_v2("test.db", &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | \
                                    SQLITE_OPEN_SHAREDCACHE , nullptr);
    if (ret != SQLITE_OK) {
        cout << "error open db" << endl;
        return nullptr;
    }

    char* errMsg2 = nullptr;
    ret = sqlite3_exec(db, "pragma journal_mode=WAL", nullptr, nullptr, &errMsg2);
    if (ret != SQLITE_OK) {
        cout << "journal_mode=WAL error " << errMsg2 << endl;
        return nullptr;
    }

    char* errMsg1 = nullptr;
    ret = sqlite3_exec(db, "pragma busy_timeout=5000", nullptr, nullptr, &errMsg1);
    if (ret != SQLITE_OK) {
        cout << "pragma timeout error " << errMsg1 << endl;
        return nullptr;
    }

    char* errMsg = NULL;
    ret = sqlite3_exec(db, create_db_sql.c_str(), nullptr, nullptr, &errMsg);
    if (ret != SQLITE_OK) {
        cout << "create db error " << errMsg << endl;
        if (errMsg) sqlite3_free(errMsg);
        return nullptr;
    }

    int safe = sqlite3_threadsafe();
    cout << "safe " << safe << endl;
    // sqlite3_busy_timeout(db, 5 * 1000000);
    
    return db;
}


int main() {
    sqlite3* db1 = NewConn();
    sqlite3* db2 = NewConn();
    sqlite3* db3 = NewConn();

    auto a = std::thread(InsertFunc, db1);
    auto b = std::thread(QueryFunc, db1);
    auto c = std::thread(QueryFunc, db1);
    a.join();
    b.join();
    c.join();
    sqlite3_close(db1);
    sqlite3_close(db2);
    sqlite3_close(db3);
}
