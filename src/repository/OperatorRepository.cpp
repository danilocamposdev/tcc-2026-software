#include "OperatorRepository.h"
#include "../database/Database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

OperatorRepository& OperatorRepository::instance() {
    static OperatorRepository inst;
    return inst;
}

Operator OperatorRepository::save(Operator op) {
	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        INSERT INTO operators (name, level)
        VALUES (:name, :level)
    )");
	query.bindValue(":name", QString::fromStdString(op.name()));
	query.bindValue(":level", op.level());

	if (!query.exec()) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}

	op.setId(query.lastInsertId().toInt());
	emit changed();

	return op;
}

Operator OperatorRepository::update(Operator op) {
    if (!op.isPersisted()) {
        qWarning() << "Cannot update operator without id";
        return op;
    }
    QSqlQuery query(Database::instance().connection());
    query.prepare("UPDATE operators SET name = :name, level = :level WHERE id = :id");
    query.bindValue(":name", QString::fromStdString(op.name()));
    query.bindValue(":level", op.level());
    query.bindValue(":id", op.id().value());
    if (!query.exec())
        qWarning() << "SQL error (update):" << query.lastError().text();
    emit changed();
    return op;
}

bool OperatorRepository::remove(int id) {
    QSqlQuery query(Database::instance().connection());
    query.prepare("DELETE FROM operators WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qWarning() << "SQL error (remove):" << query.lastError().text();
        return false;
    }
    emit changed();
    return true;
}


std::optional<Operator> OperatorRepository::get_by_id(int id) const {
	QSqlQuery query(Database::instance().connection());
	query.prepare("SELECT id, name, level FROM operators WHERE id = :id");
	query.bindValue(":id", id);
	if (!query.exec()) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}
	if (query.next()) {
		Operator op = Operator{
			query.value("name").toString().toStdString(),
				query.value("level").toInt()
		};
		op.setId(query.value("id").toInt());
		return op;
	}
	return std::nullopt;
}

std::vector<Operator> OperatorRepository::all() const {
	std::vector<Operator> operators;
	QSqlQuery query(Database::instance().connection());
	if (!query.exec("SELECT id, name, level FROM operators")) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}

	while (query.next()) {
		Operator op{
			query.value("name").toString().toStdString(),
				query.value("level").toInt()
		};
		op.setId(query.value("id").toInt());
		operators.push_back(op);
	}
	return operators;
}
