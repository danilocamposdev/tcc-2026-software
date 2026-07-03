#pragma once
#include "Section.h"
#include "../tables/TableSimple.h"
#include <QHeaderView>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "../dialogs/OrderFormDialog.h"
#include "../dialogs/EditOrderFormDialog.h"
#include "../buttons/DialogButton.h"
#include <repository/OrderRepository.h>
#include <repository/MoldRepository.h>
#include <repository/ClientRepository.h>
#include <model/Priority.h>
#include <QStackedWidget>
#include <QTimer>
#include "../buttons/ExportButton.h"

class SectionOrders : public Section {
	Q_OBJECT

	public:
		SectionOrders(QWidget *parent = nullptr)
			: Section(QIcon(":icons/sticky.svg"), "Pedidos", parent)
		{
			createTable();
			connect(mTable, &QTableWidget::cellClicked, this, [this](int row, int col) {
					int id = mTable->item(row, 0)->text().toInt();
					EditOrderFormDialog dialog(id, this);
					dialog.exec();
					});

			auto exportButton = new ExportButton("Pedidos", true);
			exportButton->setTable(mTable);
			addButton(exportButton);

			addButton(new DialogButton<OrderFormDialog>(QIcon(":/icons/plus.svg"),"Adicionar"));

			connect(&OrderRepository::instance(), &OrderRepository::changed, this, &SectionOrders::reload);
			connect(&MoldRepository::instance(), &MoldRepository::changed, this, &SectionOrders::reload);
			connect(&ClientRepository::instance(), &ClientRepository::changed, this, &SectionOrders::reload);
			reload();
		}

	private:
		QStackedWidget *mStack;
		QLabel *mLoadingLabel;
		TableSimple *mTable;
		void createTable() {

			QStringList headers = {"ID", "Data de entrega", "Matriz", "Quantidade", "Cliente", "Prioridade", "Quantidade Produzida"};
			mTable = new TableSimple(1, headers.size());
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
					auto orders = OrderRepository::instance().all();
					populateTable(orders);
					mStack->setCurrentWidget(mTable);
					});
		}
		void populateTable(const std::vector<Order> &orders) {
			mTable->setRowCount(static_cast<int>(orders.size()));
			int row = 0;
			for (const Order &order : orders) {
				if(order.isPersisted()) {
					mTable->setItem(row, 0, new QTableWidgetItem(QString::number(order.id().value())));
					mTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(order.delivery_date().to_string())));
					mTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(MoldRepository::instance().get_by_id(order.mold_id()).value().type())));
					mTable->setItem(row, 3, new QTableWidgetItem(QString::number(order.quantity())));
					mTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(ClientRepository::instance().get_by_id(order.client_id()).value().name())));
					//mTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString("******")));
					mTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(Priority::text(order.priority()))));
					mTable->setItem(row, 6, new QTableWidgetItem(QString::number(order.produced_quantity())));
				}
				++row;
			}
			mTable->resizeColumnsToContents();
			mTable->resizeRowsToContents();

			mTable->setColumnHidden(0, true);

		}
};
