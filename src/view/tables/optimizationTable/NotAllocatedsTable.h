#pragma once
#include <QTableWidget>
#include <QHeaderView>
#include <repository/OptimizationConfigRepository.h>
#include <repository/OperatorRepository.h>
#include <repository/ProductionAllocationRepository.h>
#include "../../../presenter/OptimizationTablePresenter.h"
#include "../Table.h"
#include "./OptimizationCellWidget.h"

class NotAllocatedsTable : public Table {
	Q_OBJECT

	public:
		NotAllocatedsTable(QWidget *parent = nullptr) : Table(parent) {
			connect(&ProductionAllocationRepository::instance(), &ProductionAllocationRepository::changed, this, &NotAllocatedsTable::reload);
			connect(&OptimizationConfigRepository::instance(), &OptimizationConfigRepository::changed, this, &NotAllocatedsTable::reload);
			connect(&OperatorRepository::instance(), &OperatorRepository::changed, this, &NotAllocatedsTable::reload);
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

			if (vm.horizontalHeaders.isEmpty()) {
				setColumnCount(0);
				setRowCount(0);
				return;
			}

			int notAllocatedCol = vm.horizontalHeaders.size() - 1;

			// junta todos os jobs da coluna "Não alocados", ignorando qual operador
			// "ganhou" a variável no dia T (não importa pra essa tabela)
			std::vector<OptimizationTableViewModel::JobCardData> jobs;
			for (const auto& cell : vm.cells) {
				if (cell.col != notAllocatedCol) continue;
				for (const auto& job : cell.jobs)
					jobs.push_back(job);
			}

			setColumnCount(1);
			setHorizontalHeaderLabels({vm.horizontalHeaders.last()});

			setRowCount(static_cast<int>(jobs.size()));
			QStringList verticalHeaders;
			for (size_t i = 0; i < jobs.size(); ++i)
				verticalHeaders.append("");
			setVerticalHeaderLabels(verticalHeaders);

			for (size_t i = 0; i < jobs.size(); ++i) {
				setCellWidget(static_cast<int>(i), 0, new OptimizationCellWidget({jobs[i]}, this));
			}

			setEditTriggers(QAbstractItemView::NoEditTriggers);
			setSelectionMode(QAbstractItemView::NoSelection);
			setMouseTracking(false);

			verticalHeader()->hide();

			horizontalHeader()->setStretchLastSection(true);
			resizeRowsToContents();
		}
};
