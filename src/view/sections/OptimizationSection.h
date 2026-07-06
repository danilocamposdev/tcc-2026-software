#pragma once
#include "Section.h"
#include <QHeaderView>
#include <QToolButton>
#include <QMenu>
#include <repository/OptimizationConfigRepository.h>
#include <repository/ProductionAllocationRepository.h>
#include <optimization/ProblemData.h>
#include <optimization/AllocationOptimizer.h>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QString>
#include "../tables/optimizationTable/OptimizationTable.h"
#include "../tables/optimizationTable/NotAllocatedsTable.h"
#include "../dialogs/OptimizationFormDialog.h"
#include "../buttons/DialogButton.h"
#include "../buttons/ExportButton.h"

class OptimizationSection : public Section {
	Q_OBJECT

	private:
		QStackedWidget *mStack = nullptr;
		QWidget *mContentWidget = nullptr;
		OptimizationTable *mTable = nullptr;
		NotAllocatedsTable *mNotAllocatedTable = nullptr;
		ExportButton *mExportButton = nullptr;

	public:
		OptimizationSection(QWidget *parent = nullptr)
			: Section(QIcon(":icons/rocket.svg"), "Otimização", parent)
		{
			mExportButton = new ExportButton("Otimização", true);
			mExportButton->setEnabled(false);
			addButton(mExportButton);

			Button *clearButton = new Button(QIcon(":/icons/x.svg"), "Limpar");
			connect(clearButton, &QPushButton::clicked, this, [this]() {
					ProductionAllocationRepository::instance().clear();
					});
			addButton(clearButton);

			DialogButton<OptimizationFormDialog> *optimizerButton = new DialogButton<OptimizationFormDialog>(QIcon(":/icons/play.svg"),"Otimizar");
			optimizerButton->dialog()->onAccepted([this, optimizerButton]() {
					auto *d = optimizerButton->dialog();
					optimize(
							d->dateValue(0),
							d->intValue(1),
							d->intValue(2)
							);
					});
			addButton(optimizerButton);

			mStack = new QStackedWidget();
			mStack->addWidget(new QLabel()); // index 0

			auto loadingLabel = new QLabel("Carregando...");
			loadingLabel->setAlignment(Qt::AlignCenter);
			mStack->addWidget(loadingLabel);  // index 1

			mStack->setCurrentWidget(mStack->widget(0));
			setContent(mStack);

			connect(&ProductionAllocationRepository::instance(), &ProductionAllocationRepository::changed, this, &OptimizationSection::reload);
			connect(&OptimizationConfigRepository::instance(), &OptimizationConfigRepository::changed, this, &OptimizationSection::reload);
			reload();
		}

		private slots:
			void optimize(Date startDate, int horizon, int dailyCapacity) {
				ProblemData problem{&MoldRepository::instance(), &OperatorRepository::instance(), &OrderRepository::instance(), startDate, horizon};
				AllocationOptimizer optimizer{problem};
				optimizer.solve();
				optimizer.print_results();
				optimizer.save_results(&OptimizationConfigRepository::instance(), &ProductionAllocationRepository::instance());
			}

		void reload() {
			if (!OptimizationConfigRepository::instance().current().has_value()) {
				mStack->setCurrentWidget(mStack->widget(0));
				mExportButton->setEnabled(false);
				return;
			}

			mStack->setCurrentWidget(mStack->widget(1));
			QTimer::singleShot(0, this, [this]() {
					auto productions = ProductionAllocationRepository::instance().all();

					updateVisual();

					if (productions.empty()) {
					mStack->setCurrentWidget(mStack->widget(0));
					mExportButton->setEnabled(false);
					} else {
					mStack->setCurrentWidget(mStack->widget(2));
					mExportButton->setEnabled(true);
					}
					});
		}

	private:

		void updateVisual() {
			auto config = OptimizationConfigRepository::instance().current();
			if (!config.has_value()) return;

			if (!mTable) {
				mTable = new OptimizationTable();
			}
			if (!mNotAllocatedTable) {
				mNotAllocatedTable = new NotAllocatedsTable();
			}

			auto *infoWidget = new QWidget();
			auto *infoLayout = new QVBoxLayout(infoWidget);
			infoLayout->setContentsMargins(0, 0, 0, 8);
			infoLayout->setSpacing(2);

			QDate date{config->start_date().date()};
			infoLayout->addWidget(new QLabel(
						QString("Data de início: %1").arg(date.toString("dd/MM/yyyy"))));
			infoLayout->addWidget(new QLabel(
						QString("Períodos: %1 dias").arg(config->horizon())));

			auto *tablesWidget = new QWidget();
			auto *tablesLayout = new QHBoxLayout(tablesWidget);
			tablesLayout->setContentsMargins(0, 0, 0, 0);

			mNotAllocatedTable->setFixedWidth(280);
			mNotAllocatedTable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

			mTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

			tablesLayout->addWidget(mTable);
			tablesLayout->addWidget(mNotAllocatedTable);

			auto *contentWidget = new QWidget();
			auto *contentLayout = new QVBoxLayout(contentWidget);
			contentLayout->setContentsMargins(0, 0, 0, 0);
			contentLayout->addWidget(infoWidget);
			contentLayout->addWidget(tablesWidget);

			if (mContentWidget) {
				mStack->removeWidget(mContentWidget);
				mContentWidget->deleteLater();
			}

			mContentWidget = contentWidget;
			mStack->addWidget(mContentWidget);
			mExportButton->setTable(mTable);
		}
};
