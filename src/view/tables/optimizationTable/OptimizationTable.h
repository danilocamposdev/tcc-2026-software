#pragma once
#include <QTableWidget>
#include <QMouseEvent>
#include <QHeaderView>
#include <QDate>
#include <repository/OptimizationConfigRepository.h>
#include <repository/OperatorRepository.h>
#include <repository/ProductionAllocationRepository.h>
#include <repository/MoldRepository.h>
#include <repository/OrderRepository.h>
#include <repository/ClientRepository.h>
#include <model/Priority.h>
#include "../../../presenter/OptimizationTablePresenter.h"
#include "../Table.h"
#include "./OptimizationCellWidget.h"

class OptimizationTable : public Table {
	Q_OBJECT

	public:
		OptimizationTable(QWidget *parent = nullptr) : Table(parent) {
			connect(&ProductionAllocationRepository::instance(), &ProductionAllocationRepository::changed, this, &OptimizationTable::reload);
			connect(&OptimizationConfigRepository::instance(), &OptimizationConfigRepository::changed, this, &OptimizationTable::reload);
			connect(&OperatorRepository::instance(), &OperatorRepository::changed, this, &OptimizationTable::reload);
			reload();
		}

		private slots:
			void reload() {
				auto vm = OptimizationTablePresenter{}.present();
				render(vm);
			}

	private:

		void render(const OptimizationTableViewModel& vm) {
			clear();

			// exclui a última coluna ("Não alocados") — ela vive na NotAllocatedsTable agora
			int allocatedCols = vm.horizontalHeaders.isEmpty() ? 0 : vm.horizontalHeaders.size() - 1;

			QStringList allocatedHeaders = vm.horizontalHeaders;
			if (!allocatedHeaders.isEmpty())
				allocatedHeaders.removeLast();

			setColumnCount(allocatedCols);
			setHorizontalHeaderLabels(allocatedHeaders);
			setRowCount(vm.verticalHeaders.size());
			setVerticalHeaderLabels(vm.verticalHeaders);

			for (const auto& cell : vm.cells) {
				if (cell.col >= allocatedCols) continue; // pula jobs do dia T
				setCellWidget(cell.row, cell.col, new OptimizationCellWidget(cell.jobs, this));
			}

			setEditTriggers(QAbstractItemView::NoEditTriggers);
			setSelectionMode(QAbstractItemView::NoSelection);
			setMouseTracking(false);

			resizeColumnsToContents();

			int maxColWidth = 0;
			for (int c = 0; c < columnCount(); ++c)
				maxColWidth = std::max(maxColWidth, columnWidth(c));
			for (int c = 0; c < columnCount(); ++c)
				setColumnWidth(c, maxColWidth);

			resizeRowsToContents();
		}


};
