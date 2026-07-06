#pragma once
#include "Section.h"
#include "../tables/TableSimple.h"
#include <QHeaderView>
#include <QStackedWidget>
#include <QLabel>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include "../dialogs/OperatorFormDialog.h"
#include "../dialogs/EditOperatorFormDialog.h"
#include "../buttons/DialogButton.h"
#include "../buttons/ExportButton.h"
#include <repository/OperatorRepository.h>
#include <model/Mold.h>

class SectionOperators : public Section {
	public:
		SectionOperators(QWidget *parent = nullptr)
			: Section(QIcon(":icons/operator.svg"), "Operadores", parent)
	{
		createTable();
		connect(mTable, &QTableWidget::cellClicked, this, [this](int row, int col) {
					int id = mTable->item(row, 0)->text().toInt();
					EditOperatorFormDialog dialog(id, this);
					dialog.exec();
					});

		auto exportButton = new ExportButton("Operadores", true);
			exportButton->setTable(mTable);
			addButton(exportButton);

		addButton(new DialogButton<OperatorFormDialog>(QIcon(":/icons/plus.svg"), "Adicionar"));

		connect(&OperatorRepository::instance(), &OperatorRepository::changed, this, &SectionOperators::reload);

		reload();
	}

	private:
		QStackedWidget *mStack;
		QLabel *mLoadingLabel;
		TableSimple *mTable;

		void createTable() {
			QStringList headers = {"ID", "Nome", "Nível"};

			mTable = new TableSimple(0, headers.size());
			mTable->setHorizontalHeaderLabels(headers);

			mLoadingLabel = new QLabel("Carregando...");
			mLoadingLabel->setAlignment(Qt::AlignCenter);

			mStack = new QStackedWidget();
			mStack->addWidget(mLoadingLabel); // index 0
			mStack->addWidget(mTable);        // index 1

			setContent(mStack);
		}

		void reload() {
			mStack->setCurrentWidget(mLoadingLabel);

			QTimer::singleShot(0, this, [this]() {
					auto operators = OperatorRepository::instance().all();
					populateTable(operators);
					mStack->setCurrentWidget(mTable);
					});
		}
		void populateTable(const std::vector<Operator> &operators) {
			mTable->setRowCount(static_cast<int>(operators.size()));
			int row = 0;
			for (const Operator &op : operators) {
				if(op.isPersisted()) {
					mTable->setItem(row, 0, new QTableWidgetItem(QString::number(op.id().value())));
					mTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(op.name())));
					mTable->setItem(row, 2, new QTableWidgetItem(QString::number(op.level())));
				}
				++row;
			}
			mTable->resizeColumnsToContents();
			mTable->resizeRowsToContents();

			mTable->setColumnHidden(0, true);

		}
};
