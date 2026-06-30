#pragma once
#include "Section.h"
#include "../tables/TableSimple.h"
#include <QHeaderView>
#include <QStackedWidget>
#include <QLabel>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include "../dialogs/MoldFormDialog.h"
#include "../dialogs/EditMoldFormDialog.h"
#include "../buttons/DialogButton.h"
#include <repository/MoldRepository.h>
#include <model/Mold.h>

class SectionMolds : public Section {
	public:
		SectionMolds(QWidget *parent = nullptr)
			: Section(QIcon(":icons/table.svg"), "Matrizes", parent)
	{
		createTable();

		connect(mTable, &QTableWidget::cellClicked, this, [this](int row, int col) {
					int id = mTable->item(row, 0)->text().toInt();
					EditMoldFormDialog dialog(id, this);
					dialog.exec();
					});

		addButton(new DialogButton<MoldFormDialog>(QIcon(":/icons/plus.svg"), "Adicionar"));

		connect(&MoldRepository::instance(), &MoldRepository::changed, this, &SectionMolds::reload);

		reload();
	}

	private:
		QStackedWidget *mStack;
		QLabel *mLoadingLabel;
		TableSimple *mTable;

		void createTable() {
			QStringList headers = {"ID", "Tipo", "Quantidade Disponível"};

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
					auto molds = MoldRepository::instance().all();
					populateTable(molds);
					mStack->setCurrentWidget(mTable);
					});
		}
		void populateTable(const std::vector<Mold> &molds) {
			mTable->setRowCount(static_cast<int>(molds.size()));
			int row = 0;
			for (const Mold &mold : molds) {
				if(mold.isPersisted()) {
					mTable->setItem(row, 0, new QTableWidgetItem(QString::number(mold.id().value())));
					mTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(mold.type())));
					mTable->setItem(row, 2, new QTableWidgetItem(QString::number(mold.available_quantity())));
				}
				++row;
			}
			mTable->resizeColumnsToContents();
			mTable->resizeRowsToContents();

			mTable->setColumnHidden(0, true);

		}
};
