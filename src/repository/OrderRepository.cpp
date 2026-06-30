#include "OrderRepository.h"
#include "../database/Database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <model/Priority.h>

OrderRepository& OrderRepository::instance() {
	static OrderRepository inst;
	return inst;
}

Order OrderRepository::save(Order order) {
	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        INSERT INTO orders (delivery_date, mold_id, quantity, client_id, priority, produced_quantity)
        VALUES (:delivery_date, :mold_id, :quantity, :client_id, :priority, :produced_quantity)
    )");
	auto d = order.delivery_date().date();
	QString date = QString("%1-%2-%3")
		.arg(int(d.year()), 4, 10, QChar('0'))
		.arg(unsigned(d.month()), 2, 10, QChar('0'))
		.arg(unsigned(d.day()), 2, 10, QChar('0'));
	query.bindValue(":delivery_date", date);
	query.bindValue(":mold_id", order.mold_id());
	query.bindValue(":quantity", order.quantity());
	query.bindValue(":client_id", order.client_id());
	query.bindValue(":priority", order.priority());
	query.bindValue(":produced_quantity", order.produced_quantity());

	if (!query.exec()) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}

	order.setId(query.lastInsertId().toInt());
	emit changed();
	return order;
}

Order OrderRepository::update(Order order) {
	if (!order.isPersisted()) {
		qWarning() << "Cannot update order without id";
		return order;
	}
	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        UPDATE orders SET
            delivery_date     = :delivery_date,
            mold_id           = :mold_id,
            quantity          = :quantity,
            client_id         = :client_id,
            priority          = :priority,
            produced_quantity = :produced_quantity
        WHERE id = :id
    )");
	auto d = order.delivery_date().date();
	QString date = QString("%1-%2-%3")
		.arg(int(d.year()), 4, 10, QChar('0'))
		.arg(unsigned(d.month()), 2, 10, QChar('0'))
		.arg(unsigned(d.day()), 2, 10, QChar('0'));
	query.bindValue(":delivery_date", date);
	query.bindValue(":mold_id", order.mold_id());
	query.bindValue(":quantity", order.quantity());
	query.bindValue(":client_id", order.client_id());
	query.bindValue(":priority", order.priority());
	query.bindValue(":produced_quantity", order.produced_quantity());
	query.bindValue(":id", order.id().value());
	if (!query.exec())
		qWarning() << "SQL error (update):" << query.lastError().text();
	emit changed();
	return order;
}

bool OrderRepository::remove(int id) {
	QSqlQuery query(Database::instance().connection());
	query.prepare("DELETE FROM orders WHERE id = :id");
	query.bindValue(":id", id);
	if (!query.exec()) {
		qWarning() << "SQL error (remove):" << query.lastError().text();
		return false;
	}
	emit changed();
	return true;
}

std::optional<Order> OrderRepository::get_by_id(int id) const {
	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        SELECT id, delivery_date, mold_id, quantity, client_id, priority, produced_quantity
        FROM orders WHERE id = :id
    )");
	query.bindValue(":id", id);
	if (!query.exec()) {
		qWarning() << "SQL error (add):" << query.lastError().text();
	}

	if (query.next()) {
		auto date_str = query.value("delivery_date").toString().split("-");
		Date delivery_date{
			date_str[0].toInt(),
				static_cast<unsigned int>(date_str[1].toInt()),
				static_cast<unsigned int>(date_str[2].toInt())
		};
		Order order {
			delivery_date,
				query.value("mold_id").toInt(),
				query.value("client_id").toInt(),
				query.value("quantity").toInt(),
				query.value("priority").toInt(),
				query.value("produced_quantity").toInt()
		};
		order.setId(query.value("id").toInt());
		return order;
	}
	return std::nullopt;
}

std::vector<Order> OrderRepository::all() const {
	std::vector<Order> orders;
	QSqlQuery query(Database::instance().connection());
	query.exec(R"(
        SELECT id, delivery_date, mold_id, quantity, client_id, priority, produced_quantity
        FROM orders
    )");
	while (query.next()) {
		auto date_str = query.value("delivery_date").toString().split("-");
		Date delivery_date{
			date_str[0].toInt(),
				static_cast<unsigned int>(date_str[1].toInt()),
				static_cast<unsigned int>(date_str[2].toInt())
		};
		Order order {
			delivery_date,
				query.value("mold_id").toInt(),
				query.value("client_id").toInt(),
				query.value("quantity").toInt(),
				query.value("priority").toInt(),
				query.value("produced_quantity").toInt()
		};
		order.setId(query.value("id").toInt());
		orders.push_back(order);
	}
	return orders;
}
