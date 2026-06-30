#pragma once
#include "Section.h"
#include "../tables/TableOptimization.h"
#include <QHeaderView>
#include <QToolButton>
#include <QMenu>
#include <repository/ClientRepository.h>
#include <repository/OrderRepository.h>
#include <repository/MoldRepository.h>
#include <repository/OptimizationConfigRepository.h>
#include <repository/ProductionAllocationRepository.h>
#include <optimization/ProblemData.h>
#include <optimization/AllocationOptimizer.h>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include "../dialogs/OptimizationFormDialog.h"
#include "../buttons/DialogButton.h"
#include <model/Priority.h>

class SectionOptimization : public Section {
	Q_OBJECT

	private:
		ProductionAllocationRepository    *mProductionAllocationRepository;
		QStackedWidget                    *mStack        = nullptr;
		QLabel                            *mLoadingLabel = nullptr;
		TableOptimization                 *mTable        = nullptr;

	public:
		SectionOptimization(
				ProductionAllocationRepository *productionAllocationRepository,
				QWidget *parent = nullptr)
			: Section(QIcon(":icons/rocket.svg"), "Otimização", parent),
			mProductionAllocationRepository{productionAllocationRepository}
		{
			Button *exportButton = new Button(QIcon(":/icons/file-earmark.svg"), "Exportar");
			QMenu *exportMenu = new QMenu();
			exportMenu->addAction(QIcon(":/icons/filetype-csv.svg"), "CSV");
			exportMenu->addAction(QIcon(":/icons/file-earmark-pdf.svg"), "PDF");
			exportButton->setMenu(exportMenu);
			addButton(exportButton);

			auto *optimizationFormDialog = new OptimizationFormDialog();

			DialogButton<OptimizationFormDialog> *optimizerButton = new DialogButton<OptimizationFormDialog>(QIcon(":/icons/play.svg"),"Otimizar");

			optimizerButton->dialog()->onAccepted([this, optimizerButton]() {
					auto *d = optimizerButton->dialog();
					optimize(
							d->dateValue(0),   // Data de início
							d->intValue(1),    // Períodos
							d->intValue(2)     // Capacidade diária
							);
					});

			//QPushButton *optimizerButton = new QPushButton(QIcon(":/icons/play.svg"), "Otimizar");
			//connect(optimizerButton, &QPushButton::clicked, this, &SectionOptimization::optimize);
			addButton(optimizerButton);

			if (OptimizationConfigRepository::instance().current().has_value()) {
				createTable();
				auto productions = mProductionAllocationRepository->all();
				populateTable(productions);
				mStack->setCurrentWidget(mStack->widget(1)); // índice 1 = conteúdo
			}
		}

		private slots:
			void optimize(Date startDate, int horizon, int dailyCapacity) {
				ProblemData problem{&MoldRepository::instance(), &OrderRepository::instance(), startDate, horizon, dailyCapacity};
				AllocationOptimizer optimizer{problem};
				optimizer.solve();
				optimizer.print_results();
				optimizer.save_results(&OptimizationConfigRepository::instance(), mProductionAllocationRepository);

				reload();
			}

		void reload() {
			if (!OptimizationConfigRepository::instance().current().has_value()) return;

			if (mStack) mStack->setCurrentWidget(mLoadingLabel);

			createTable();

			QTimer::singleShot(0, this, [this]() {
					auto productions = mProductionAllocationRepository->all();
					populateTable(productions);
					mStack->setCurrentWidget(mStack->widget(1));
					});
		}

	private:
		void createTable() {
			auto config = OptimizationConfigRepository::instance().current();
			if (!config.has_value()) return;

			delete mStack;
			mStack        = nullptr;
			mTable        = nullptr;
			mLoadingLabel = nullptr;

			QDate date{config->start_date().date()};
			mTable = new TableOptimization(date, config->daily_capacity(), config->horizon()+1);

			// labels de config
			auto *infoWidget = new QWidget();
			auto *infoLayout = new QVBoxLayout(infoWidget);
			infoLayout->setContentsMargins(0, 0, 0, 8);
			infoLayout->setSpacing(2);

			infoLayout->addWidget(new QLabel(
						QString("Data de início: %1").arg(date.toString("dd/MM/yyyy"))));
			infoLayout->addWidget(new QLabel(
						QString("Períodos: %1 dias").arg(config->horizon())));
			infoLayout->addWidget(new QLabel(
						QString("Capacidade diária: %1 unidades").arg(config->daily_capacity())));

			// layout principal — info + tabela
			auto *contentWidget = new QWidget();
			auto *contentLayout = new QVBoxLayout(contentWidget);
			contentLayout->setContentsMargins(0, 0, 0, 0);
			contentLayout->addWidget(infoWidget);
			contentLayout->addWidget(mTable);

			// loading label
			mLoadingLabel = new QLabel("Carregando...");
			mLoadingLabel->setAlignment(Qt::AlignCenter);

			mStack = new QStackedWidget();
			mStack->addWidget(mLoadingLabel);  // índice 0
			mStack->addWidget(contentWidget);  // índice 1

			setContent(mStack);
		}

		void populateTable(const std::vector<ProductionAllocation>& productions) {
			auto config = OptimizationConfigRepository::instance().current();
			if (!config.has_value()) return;

			QDate start{config->start_date().date()};
			std::vector<std::vector<const ProductionAllocation*>> prod_by_day(config->horizon()+1);
			for (const auto& p : productions) {
				int col = start.daysTo(QDate{p.date().date()});
				if (col >= 0 && col <= config->horizon())
					prod_by_day[col].push_back(&p);
			}

			for (auto& day : prod_by_day) {
				std::sort(day.begin(), day.end(), [this](const ProductionAllocation* a, const ProductionAllocation* b) {
						auto orderA = OrderRepository::instance().get_by_id(a->order_id());
						auto orderB = OrderRepository::instance().get_by_id(b->order_id());

						if (orderA->delivery_date().date() != orderB->delivery_date().date()){
						return orderA->delivery_date().date() < orderB->delivery_date().date();
						}

						return orderA->priority() > orderB->priority();
						});
			}
			int unitsInDayT = 0;
			for (const auto* p : prod_by_day.back())
				unitsInDayT += p->units();

			if (unitsInDayT > mTable->rowCount())
				mTable->setRowCount(unitsInDayT);

			int col = 0;
			for (const auto& prods : prod_by_day) {
				int row = 0;
				for (const auto* p : prods) {
					for(int unit = 1; unit <= p->units(); unit++) {
						auto order  = OrderRepository::instance().get_by_id(p->order_id());
						auto mold   = MoldRepository::instance().get_by_id(order->mold_id());
						auto client = ClientRepository::instance().get_by_id(order->client_id());

						auto start_date = config->start_date();
						int deadline = order->delivery_date().to_int(&start_date);
						int delay = std::max(0, col +1 - deadline);

						QString text = QString("Matriz: %1\nCliente: %2\nPrioridade: %3\nEntrega: %4\nAtraso (dias): %5")
							.arg(QString::fromStdString(mold->type()))
							.arg(QString::fromStdString(client->name()))
							.arg(QString::fromStdString(Priority::text(order->priority())))
							.arg(QString::fromStdString(order->delivery_date().to_string()))
							.arg(delay);
						mTable->setItem(row, col, new QTableWidgetItem(text));
						++row;
					}
				}
				++col;
			}

			mTable->resizeColumnsToContents();

			int maxColWidth = 0;
			for (int c = 0; c < mTable->columnCount(); ++c)
				maxColWidth = std::max(maxColWidth, mTable->columnWidth(c));

			for (int c = 0; c < mTable->columnCount(); ++c)
				mTable->setColumnWidth(c, maxColWidth);

			// só agora, com a largura final das colunas, calcula a altura das linhas
			mTable->resizeRowsToContents();

			int maxRowHeight = 0;
			for (int r = 0; r < mTable->rowCount(); ++r)
				maxRowHeight = std::max(maxRowHeight, mTable->rowHeight(r));

			for (int r = 0; r < mTable->rowCount(); ++r)
				mTable->setRowHeight(r, maxRowHeight);

			mTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
			mTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		}
};
