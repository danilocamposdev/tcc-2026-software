#include "Database.h"
#include <QSqlQuery>

Database::Database() {
	db_ = QSqlDatabase::addDatabase("QSQLITE");
	db_.setDatabaseName("data.db");
	db_.open();

	QSqlQuery query(db_);
	query.exec("PRAGMA foreign_keys = ON");
	query.exec(R"(
        CREATE TABLE IF NOT EXISTS clients (
            id   INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL
        )
    )");
	query.exec(R"(
        CREATE TABLE IF NOT EXISTS molds (
            id                 INTEGER PRIMARY KEY AUTOINCREMENT,
            type               TEXT NOT NULL,
            available_quantity INTEGER NOT NULL
        )
    )");
	query.exec(R"(
        CREATE TABLE IF NOT EXISTS orders (
            id                INTEGER PRIMARY KEY AUTOINCREMENT,
            delivery_date     TEXT NOT NULL,
            mold_id           INTEGER NOT NULL,
            quantity          INTEGER NOT NULL,
            client_id         INTEGER NOT NULL,
            priority          INTEGER NOT NULL CHECK(priority BETWEEN 1 AND 3),
            produced_quantity INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY (mold_id)   REFERENCES molds(id)   ON DELETE RESTRICT,
            FOREIGN KEY (client_id) REFERENCES clients(id) ON DELETE RESTRICT
        )
    )");
	query.exec(R"(
        CREATE TABLE IF NOT EXISTS optimization_configs (
            id             INTEGER PRIMARY KEY AUTOINCREMENT,
            start_date     TEXT NOT NULL,
            horizon        INTEGER NOT NULL,
            daily_capacity INTEGER NOT NULL
        )
    )");
	query.exec(R"(
        CREATE TABLE IF NOT EXISTS production_allocations (
            id                     INTEGER PRIMARY KEY AUTOINCREMENT,
            optimization_config_id INTEGER NOT NULL,
            order_id               INTEGER NOT NULL,
            day                    INTEGER NOT NULL,
            units                  INTEGER NOT NULL,
            FOREIGN KEY (optimization_config_id) REFERENCES optimization_configs(id) ON DELETE CASCADE,
            FOREIGN KEY (order_id)               REFERENCES orders(id)               ON DELETE RESTRICT
        )
    )");
}

Database& Database::instance() {
	static Database instance;
	return instance;
}

QSqlDatabase& Database::connection() {
	return db_;
}
