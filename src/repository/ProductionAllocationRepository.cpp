#include "ProductionAllocationRepository.h"
#include "../database/Database.h"
#include <QSqlQuery>
#include <QSqlError>

ProductionAllocationRepository& ProductionAllocationRepository::instance() {
	static ProductionAllocationRepository inst;
	return inst;
}

ProductionAllocation ProductionAllocationRepository::save(ProductionAllocation allocation) {
	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        INSERT INTO production_allocations (optimization_config_id, order_id, operator_id, day, units)
        VALUES (:optimization_config_id, :order_id, :operator_id, :day, :units)
    )");
	query.bindValue(":optimization_config_id", allocation.optimization_config_id());
	query.bindValue(":order_id", allocation.order_id());
	query.bindValue(":operator_id", allocation.operator_id());
	query.bindValue(":day", allocation.day());
	query.bindValue(":units", allocation.units());
	if (!query.exec())
		qWarning() << "SQL error (save production_allocation):" << query.lastError().text();
	allocation.setId(query.lastInsertId().toInt());
	emit changed();
	return allocation;
}

std::vector<ProductionAllocation> ProductionAllocationRepository::all() const {
	std::vector<ProductionAllocation> allocations;
	QSqlQuery query(Database::instance().connection());
	if (!query.exec("SELECT id, optimization_config_id, order_id, operator_id, day, units FROM production_allocations"))
		qWarning() << "SQL error (all production_allocations):" << query.lastError().text();
	while (query.next()) {
		ProductionAllocation allocation{
			query.value("optimization_config_id").toInt(),
				query.value("order_id").toInt(),
				query.value("operator_id").toInt(),
				query.value("day").toInt(),
				query.value("units").toInt()
		};
		allocation.setId(query.value("id").toInt());
		allocations.push_back(allocation);
	}
	return allocations;
}

std::vector<ProductionAllocation> ProductionAllocationRepository::by_order(int order_id) const {
	std::vector<ProductionAllocation> allocations;
	QSqlQuery query(Database::instance().connection());
	query.prepare("SELECT id, optimization_config_id, order_id, operator_id, day, units FROM production_allocations WHERE order_id = :order_id");
	query.bindValue(":order_id", order_id);
	if (!query.exec())
		qWarning() << "SQL error (by_order production_allocations):" << query.lastError().text();
	while (query.next()) {
		ProductionAllocation allocation{
			query.value("optimization_config_id").toInt(),
				query.value("order_id").toInt(),
				query.value("operator_id").toInt(),
				query.value("day").toInt(),
				query.value("units").toInt()
		};
		allocation.setId(query.value("id").toInt());
		allocations.push_back(allocation);
	}
	return allocations;
}

void ProductionAllocationRepository::clear() {
	QSqlQuery query(Database::instance().connection());
	if (!query.exec("DELETE FROM production_allocations"))
		qWarning() << "SQL error (clear production_allocations):" << query.lastError().text();
	emit changed();
}
