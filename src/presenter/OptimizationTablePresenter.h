#pragma once
#include <algorithm>
#include <repository/OptimizationConfigRepository.h>
#include <repository/OperatorRepository.h>
#include <repository/ProductionAllocationRepository.h>
#include <repository/MoldRepository.h>
#include <repository/OrderRepository.h>
#include <repository/ClientRepository.h>
#include <model/Priority.h>
#include <map>
#include <QDate>
#include "../viewModel/OptimizationTableViewModel.h"

class OptimizationTablePresenter {

	public:
		OptimizationTableViewModel present() {
			OptimizationTableViewModel vm;
			auto config = OptimizationConfigRepository::instance().current();
			if (!config.has_value()) return vm;

			std::map<int, Operator> operatorsRows;
			auto operators = OperatorRepository::instance().all();
			int opRow = 0;
			for (const auto& op : operators) {
				operatorsRows.insert({opRow, op});
				opRow++;
			}

			buildHeaders(vm, *config, operatorsRows);
			buildCells(vm, *config, operatorsRows);

			int maxRow = vm.verticalHeaders.size();
			for (const auto& c : vm.cells) maxRow = std::max(maxRow, c.row + 1);

			while (vm.verticalHeaders.size() < maxRow) {
				vm.verticalHeaders.append("");
			}

			return vm;
		}

	private:
		void buildHeaders(OptimizationTableViewModel& vm, const OptimizationConfig& config, const std::map<int, Operator>& operatorsRows) {
			auto startDate = config.start_date();
			int cols = config.horizon() + 1;
			QDate date = QDate{startDate.date()};

			for (int day = 0; day < cols; ++day) {
				if (day == cols - 1) {
					vm.horizontalHeaders.push_back("Não alocados");
				} else {
					QString dayName = QLocale().toString(date, "ddd").remove('.');
					vm.horizontalHeaders.push_back(QString("%1/%2/%3 - %4")
							.arg(date.toString("dd"))
							.arg(date.toString("MM"))
							.arg(date.toString("yy"))
							.arg(dayName));
				}
				date = date.addDays(1);
			}

			for (int i=0; i < operatorsRows.size(); i++) {
				const Operator& op = operatorsRows.at(i);
				vm.verticalHeaders.append(QString::fromStdString(op.name()) + "\nNível: " + QString::number(op.level()));
			}

		}

		void buildCells(OptimizationTableViewModel& vm, const OptimizationConfig& config, const std::map<int, Operator>& operatorsRows) const {
			auto productions = ProductionAllocationRepository::instance().all();

			std::vector<std::vector<const ProductionAllocation*>> prod_by_day(config.horizon() + 1);
			for (const auto& p : productions) {
				int col = p.day();
				if (col >= 0 && col <= config.horizon())
					prod_by_day[col].push_back(&p);
			}

			for (auto& day : prod_by_day) {
				std::sort(day.begin(), day.end(), [](const ProductionAllocation* a, const ProductionAllocation* b) {
						auto orderA = OrderRepository::instance().get_by_id(a->order_id());
						auto orderB = OrderRepository::instance().get_by_id(b->order_id());
						if (orderA->delivery_date().date() != orderB->delivery_date().date())
						return orderA->delivery_date().date() < orderB->delivery_date().date();
						return orderA->priority() > orderB->priority();
						});
			}

			std::map<std::pair<int,int>, size_t> cellIndex; // <- guarda ÍNDICE, não ponteiro

			int col = 0;
			for (const auto& prods : prod_by_day) {
				for (const auto* p : prods) {
					int targetRow = -1;
					for (const auto& [rowIndex, op] : operatorsRows) {
						if (op.id() == p->operator_id()) {
							targetRow = rowIndex;
							break;
						}
					}
					if (targetRow == -1) continue;

					auto order  = OrderRepository::instance().get_by_id(p->order_id());
					auto mold   = MoldRepository::instance().get_by_id(order->mold_id());
					auto client = ClientRepository::instance().get_by_id(order->client_id());

					auto start_date = config.start_date();
					int deadline = order->delivery_date().to_int(&start_date);
					int delay = std::max(0, col + 1 - deadline);

					OptimizationTableViewModel::JobCardData job {
						QString::fromStdString(mold->type()),
							QString::fromStdString(client->name()),
							QString::fromStdString(Priority::text(order->priority())),
							QString::fromStdString(order->delivery_date().to_string()),
							mold->duration(),
							delay,
							mold->complexity_level()
					};

					auto key = std::make_pair(targetRow, col);
					auto it = cellIndex.find(key);
					if (it == cellIndex.end()) {
						OptimizationTableViewModel::CellData cell;
						cell.row = targetRow;
						cell.col = col;
						cell.jobs.push_back(job);
						vm.cells.push_back(cell);
						cellIndex[key] = vm.cells.size() - 1; // índice, não endereço
					} else {
						vm.cells[it->second].jobs.push_back(job); // acessa via índice, sempre válido
					}
				}
				++col;
			}
			//for (const auto& cell : vm.cells) {
			//	qDebug() << "row:" << cell.row << "col:" << cell.col << "jobs:" << cell.jobs.size();
			//}
		}
};
