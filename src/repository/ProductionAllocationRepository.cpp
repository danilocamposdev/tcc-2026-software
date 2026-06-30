#include "ProductionAllocationRepository.h"
#include "../database/Database.h"
#include <QSqlQuery>
#include <QSqlError>

ProductionAllocation ProductionAllocationRepository::save(ProductionAllocation allocation) {
    QSqlQuery query(Database::instance().connection());
    query.prepare(R"(
        INSERT INTO production_allocations (optimization_config_id, order_id, date, units)
        VALUES (:optimization_config_id, :order_id, :date, :units)
    )");
    query.bindValue(":optimization_config_id", allocation.optimization_config_id());
    query.bindValue(":order_id", allocation.order_id());
    auto d = allocation.date().date();
    query.bindValue(":date", QString("%1-%2-%3")
        .arg(int(d.year()), 4, 10, QChar('0'))
        .arg(unsigned(d.month()), 2, 10, QChar('0'))
        .arg(unsigned(d.day()), 2, 10, QChar('0')));
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
    if (!query.exec("SELECT id, optimization_config_id, order_id, date, units FROM production_allocations"))
        qWarning() << "SQL error (all production_allocations):" << query.lastError().text();
    while (query.next()) {
        auto date_str = query.value("date").toString().split("-");
        Date date{
            date_str[0].toInt(),
            static_cast<unsigned int>(date_str[1].toInt()),
            static_cast<unsigned int>(date_str[2].toInt())
        };
        ProductionAllocation allocation{
            query.value("optimization_config_id").toInt(),
            query.value("order_id").toInt(),
            date,
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
    query.prepare("SELECT id, optimization_config_id, order_id, date, units FROM production_allocations WHERE order_id = :order_id");
    query.bindValue(":order_id", order_id);
    if (!query.exec())
        qWarning() << "SQL error (by_order production_allocations):" << query.lastError().text();
    while (query.next()) {
        auto date_str = query.value("date").toString().split("-");
        Date date{
            date_str[0].toInt(),
            static_cast<unsigned int>(date_str[1].toInt()),
            static_cast<unsigned int>(date_str[2].toInt())
        };
        ProductionAllocation allocation{
            query.value("optimization_config_id").toInt(),
            query.value("order_id").toInt(),
            date,
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
