#include "ClientRepository.h"
#include "../database/Database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

ClientRepository& ClientRepository::instance() {
    static ClientRepository inst;
    return inst;
}

Client ClientRepository::save(Client client) {
	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        INSERT INTO clients (name)
        VALUES (:name)
    )");
	query.bindValue(":name", QString::fromStdString(client.name()));

	if (!query.exec()) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}

	client.setId(query.lastInsertId().toInt());
	emit changed();

	return client;
}

Client ClientRepository::update(Client client) {
    if (!client.isPersisted()) {
        qWarning() << "Cannot update client without id";
        return client;
    }
    QSqlQuery query(Database::instance().connection());
    query.prepare("UPDATE clients SET name = :name WHERE id = :id");
    query.bindValue(":name", QString::fromStdString(client.name()));
    query.bindValue(":id", client.id().value());
    if (!query.exec())
        qWarning() << "SQL error (update):" << query.lastError().text();
    emit changed();
    return client;
}

bool ClientRepository::remove(int id) {
    QSqlQuery query(Database::instance().connection());
    query.prepare("DELETE FROM clients WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qWarning() << "SQL error (remove):" << query.lastError().text();
        return false;
    }
    emit changed();
    return true;
}

std::optional<Client> ClientRepository::get_by_id(int id) const {
	QSqlQuery query(Database::instance().connection());
	query.prepare("SELECT id, name FROM clients WHERE id = :id");
	query.bindValue(":id", id);
	if (!query.exec()) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}
	if (query.next()) {
		Client client = Client{
			query.value("name").toString().toStdString(),
		};
		client.setId(query.value("id").toInt());
		return client;
	}
	return std::nullopt;
}

std::vector<Client> ClientRepository::all() const {
	std::vector<Client> clients;
	QSqlQuery query(Database::instance().connection());
	if (!query.exec("SELECT id, name FROM clients")) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}

	while (query.next()) {
		Client client{
			query.value("name").toString().toStdString(),
		};
		client.setId(query.value("id").toInt());
		clients.push_back(client);
	}
	return clients;
}


