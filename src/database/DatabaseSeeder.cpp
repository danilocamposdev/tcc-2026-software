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

	// Mantidos exatamente como na tabela (coluna "Cliente")
	QStringList names = {
	};

	for (const auto& name : names) {
		query.bindValue(":name", name);
		query.exec();
	}
}

void DatabaseSeeder::seed_operators() {
	if (!is_empty("operators")) return;
	QSqlQuery query(Database::instance().connection());
	query.prepare("INSERT INTO operators (name, level) VALUES (:name, :level)");

	QList<QPair<QString, int>> operators = {
	};

	for (const auto& [name, level] : operators) {
		query.bindValue(":name", name);
		query.bindValue(":level", level);
		query.exec();
	}
}


void DatabaseSeeder::seed_molds() {
	if (!is_empty("molds")) return;
	QSqlQuery query(Database::instance().connection());
	query.prepare("INSERT INTO molds (type, available_quantity, complexity_level, duration) VALUES (:type, :available_quantity, :complexity_level, :duration)");

	QList<std::tuple<QString, int, int, int>> molds = {
	};

	for (const auto& [type, qty, complexity_level, duration] : molds) {
		query.bindValue(":type", type);
		query.bindValue(":available_quantity", qty);
		query.bindValue(":complexity_level", complexity_level);
		query.bindValue(":duration", duration);
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

	// { delivery_date, mold_id, quantity, client_id, priority, produced_quantity }
	QList<std::tuple<QString, int, int, int, int, int>> orders = {
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
