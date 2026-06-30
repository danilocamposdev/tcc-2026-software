#pragma once
#include "Section.h"
#include "../tables/TableSimple.h"
#include <QHeaderView>
#include <QStackedWidget>
#include <QLabel>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include "../dialogs/ClientFormDialog.h"
#include "../dialogs/EditClientFormDialog.h"
#include "../buttons/DialogButton.h"
#include <repository/ClientRepository.h>
#include <model/Client.h>

class SectionClients : public Section {

	public:
		SectionClients(QWidget *parent = nullptr)
			: Section(QIcon(":icons/table.svg"), "Clientes", parent)
		{
			createTable();

			addButton(new DialogButton<ClientFormDialog>(QIcon(":/icons/plus.svg"), "Adicionar"));

			connect(&ClientRepository::instance(), &ClientRepository::changed, this, &SectionClients::reload);

			reload();
		}

	private:
		QStackedWidget *mStack;
		QLabel *mLoadingLabel;
		TableSimple *mTable;

		void createTable() {
			QStringList headers = {"ID", "Nome"};


			mTable = new TableSimple(0, headers.size());
			mTable->setHorizontalHeaderLabels(headers);

			connect(mTable, &QTableWidget::cellClicked, this, [this](int row, int col) {
					int id = mTable->item(row, 0)->text().toInt();
					EditClientFormDialog dialog(id, this);
					dialog.exec();
					});

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
					auto clients = ClientRepository::instance().all();
					populateTable(clients);
					mStack->setCurrentWidget(mTable);
					});
		}
		void populateTable(const std::vector<Client> &clients) {
			mTable->setRowCount(static_cast<int>(clients.size()));
			int row = 0;
			for (const Client &client : clients) {
				if(client.isPersisted()) {
					mTable->setItem(row, 0, new QTableWidgetItem(QString::number(client.id().value())));
					mTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(client.name())));
				}
				++row;
			}

			mTable->resizeColumnsToContents();
			mTable->resizeRowsToContents();

			mTable->setColumnHidden(0, true);

		}
};
