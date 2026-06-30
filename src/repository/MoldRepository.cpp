#include "MoldRepository.h"
#include "../database/Database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

MoldRepository& MoldRepository::instance() {
    static MoldRepository inst;
    return inst;
}

Mold MoldRepository::save(Mold mold) {
	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        INSERT INTO molds (type, available_quantity)
        VALUES (:type, :available_quantity)
    )");
	query.bindValue(":type", QString::fromStdString(mold.type()));
	query.bindValue(":available_quantity", mold.available_quantity());

	if (!query.exec()) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}

	mold.setId(query.lastInsertId().toInt());
	emit changed();

	return mold;
}

Mold MoldRepository::update(Mold mold) {
    if (!mold.isPersisted()) {
        qWarning() << "Cannot update mold without id";
        return mold;
    }
    QSqlQuery query(Database::instance().connection());
    query.prepare("UPDATE molds SET type = :type, available_quantity = :available_quantity WHERE id = :id");
    query.bindValue(":type", QString::fromStdString(mold.type()));
    query.bindValue(":available_quantity", mold.available_quantity());
    query.bindValue(":id", mold.id().value());
    if (!query.exec())
        qWarning() << "SQL error (update):" << query.lastError().text();
    emit changed();
    return mold;
}

bool MoldRepository::remove(int id) {
    QSqlQuery query(Database::instance().connection());
    query.prepare("DELETE FROM molds WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qWarning() << "SQL error (remove):" << query.lastError().text();
        return false;
    }
    emit changed();
    return true;
}


std::optional<Mold> MoldRepository::get_by_id(int id) const {
	QSqlQuery query(Database::instance().connection());
	query.prepare("SELECT id, type, available_quantity FROM molds WHERE id = :id");
	query.bindValue(":id", id);
	if (!query.exec()) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}
	if (query.next()) {
		Mold mold = Mold{
			query.value("type").toString().toStdString(),
				query.value("available_quantity").toInt()
		};
		mold.setId(query.value("id").toInt());
		return mold;
	}
	return std::nullopt;
}

std::vector<Mold> MoldRepository::all() const {
	std::vector<Mold> molds;
	QSqlQuery query(Database::instance().connection());
	if (!query.exec("SELECT id, type, available_quantity FROM molds")) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}

	while (query.next()) {
		Mold mold{
			query.value("type").toString().toStdString(),
				query.value("available_quantity").toInt()
		};
		mold.setId(query.value("id").toInt());
		molds.push_back(mold);
	}
	return molds;
}
