#include "OptimizationConfigRepository.h"
#include "../database/Database.h"
#include <QSqlQuery>
#include <QSqlError>

OptimizationConfigRepository& OptimizationConfigRepository::instance() {
	static OptimizationConfigRepository inst;
	return inst;
}


OptimizationConfig OptimizationConfigRepository::save(OptimizationConfig config) {
	QSqlQuery query(Database::instance().connection());
	query.prepare(R"(
        INSERT INTO optimization_configs (start_date, horizon, daily_capacity)
        VALUES (:start_date, :horizon, :daily_capacity)
    )");
	auto d = config.start_date().date();
	query.bindValue(":start_date", QString("%1-%2-%3")
			.arg(int(d.year()), 4, 10, QChar('0'))
			.arg(unsigned(d.month()), 2, 10, QChar('0'))
			.arg(unsigned(d.day()), 2, 10, QChar('0')));
	query.bindValue(":horizon", config.horizon());
	query.bindValue(":daily_capacity", config.daily_capacity());
	if (!query.exec())
		qWarning() << "SQL error (save optimization_config):" << query.lastError().text();
	config.setId(query.lastInsertId().toInt());
	emit changed();
	return config;
}

std::optional<OptimizationConfig> OptimizationConfigRepository::current() const {
	QSqlQuery query(Database::instance().connection());
	query.prepare("SELECT id, start_date, horizon, daily_capacity FROM optimization_configs ORDER BY id DESC LIMIT 1");
	if (!query.exec())
		qWarning() << "SQL error (current optimization_config):" << query.lastError().text();
	if (query.next()) {
		auto date_str = query.value("start_date").toString().split("-");
		Date start_date{
			date_str[0].toInt(),
				static_cast<unsigned int>(date_str[1].toInt()),
				static_cast<unsigned int>(date_str[2].toInt())
		};
		OptimizationConfig config{start_date, query.value("horizon").toInt(), query.value("daily_capacity").toInt()};
		config.setId(query.value("id").toInt());
		return config;
	}
	return std::nullopt;
}

void OptimizationConfigRepository::clear() {
	QSqlQuery query(Database::instance().connection());
	if (!query.exec("DELETE FROM optimization_configs"))
		qWarning() << "SQL error (clear optimization_configs):" << query.lastError().text();
	emit changed();
}
