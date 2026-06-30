#pragma once
#include <QTableWidget>
#include <QMouseEvent>
#include <QHeaderView>
#include <QDate>
#include "Table.h"

class TableOptimization : public Table {
	Q_OBJECT

	public:
		TableOptimization(QDate startDate, int rows, int cols, QWidget *parent = nullptr)
			: Table(rows, cols, parent)
		{
			QStringList headers;

			QDate date = startDate;
			for(int day = 0; day < cols; ++day) {
				if(day == cols -1) 
					headers.push_back("Não alocados");
				else
					headers.push_back(date.toString("dd/MM/yy"));

				date = date.addDays(1);
			}


			for (int row = 0; row < rows; row++) {
				for (int col = 0; col < cols; col++) {
					this->setItem(row, col, new QTableWidgetItem(""));
				}
			}


			setHorizontalHeaderLabels(headers);
			setEditTriggers(QAbstractItemView::NoEditTriggers);
			setSelectionMode(QAbstractItemView::NoSelection);
			setMouseTracking(false);
			//verticalHeader()->hide();
		}

};
