#include "DatabaseSeeder.h"

void DatabaseSeeder::seed() {
	seed_clients();
	seed_molds();
	seed_orders();
}

bool DatabaseSeeder::is_empty(const QString& table) {
	QSqlQuery query(Database::instance().connection());
	query.exec("SELECT COUNT(*) FROM " + table);
	if (query.next())
		return query.value(0).toInt() == 0;
	return true;
}

void DatabaseSeeder::seed_clients() {
	if (!is_empty("clients")) return;

	QSqlQuery query(Database::instance().connection());
	query.prepare("INSERT INTO clients (name) VALUES (:name)");

	QStringList names = {"Empresa Alpha", "Empresa Beta", "Empresa Gamma", "Empresa Delta"};
	for (const auto& name : names) {
		query.bindValue(":name", name);
		query.exec();
	}
}

void DatabaseSeeder::seed_molds() {
	if (!is_empty("molds")) return;

	QSqlQuery query(Database::instance().connection());
	query.prepare("INSERT INTO molds (type, available_quantity) VALUES (:type, :available_quantity)");

	QList<QPair<QString, int>> molds = {
		{"Matriz A", 3},
		{"Matriz B", 2},
		{"Matriz C", 5},
		{"Matriz D", 1}
	};

	for (const auto& [type, qty] : molds) {
		query.bindValue(":type", type);
		query.bindValue(":available_quantity", qty);
		query.exec();
	}
}

void DatabaseSeeder::seed_orders() {
	if (!is_empty("orders")) return;

	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        INSERT INTO orders (delivery_date, mold_id, quantity, client_id, priority, produced_quantity)
        VALUES (:delivery_date, :mold_id, :quantity, :client_id, :priority, :produced_quantity)
    )");

	QList<std::tuple<QString, int, int, int, int, int>> orders = {
		{"2026-07-10", 1, 2, 1, 1, 0},
		{"2026-07-15", 2, 1, 2, 2, 0},
		{"2026-07-20", 3, 3, 3, 3, 0},
		{"2026-07-25", 4, 1, 4, 1, 2},
		{"2026-08-01", 1, 2, 2, 2, 0},
	};

	for (const auto& [date, mold_id, qty, client_id, priority, produced_qty] : orders) {
		query.bindValue(":delivery_date", date);
		query.bindValue(":mold_id", mold_id);
		query.bindValue(":quantity", qty);
		query.bindValue(":client_id", client_id);
		query.bindValue(":priority", priority);
		query.bindValue(":produced_quantity", produced_qty);
		query.exec();
	}
}
