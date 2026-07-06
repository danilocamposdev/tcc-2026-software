#pragma once

#include <QString>
#include <QStringList>

struct OptimizationTableViewModel {

	struct JobCardData {
		QString moldType, clientName, priorityText, deliveryDate;
		int duration, delay, complexity;
	};

	struct CellData {
		int row, col;
		std::vector<JobCardData> jobs;
	};

	QStringList horizontalHeaders;
	QStringList verticalHeaders;
	std::vector<CellData> cells;

};
