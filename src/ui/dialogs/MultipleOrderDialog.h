#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>

class MultipleOrderDialog : public QDialog {
	Q_OBJECT

	public:
		MultipleOrderDialog(QWidget *parent = nullptr) : QDialog(parent) {
			setWindowTitle("Novo Pedido");
			setMinimumWidth(500);

			QVBoxLayout *mainLayout = new QVBoxLayout(this);

			// Cliente
			QFormLayout *form = new QFormLayout();
			QComboBox *client = new QComboBox();
			client->addItems({"Cliente A", "Cliente B"});
			form->addRow("Cliente:", client);
			mainLayout->addLayout(form);


			// Tabela
			QStringList headers = {"Data de entrega", "Matriz", "Quantidade", "Prioridade"};
			QTableWidget *table = new QTableWidget(0, headers.size());
			table->setHorizontalHeaderLabels(headers);
			table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
			mainLayout->addWidget(table);

			// Botão
			QPushButton *addRowBtn = new QPushButton("+ Adicionar linha");
			mainLayout->addWidget(addRowBtn);
			addTableRow(table);


			QObject::connect(addRowBtn, &QPushButton::clicked, [table]() {
					addTableRow(table);
					});

			// Botões
			QDialogButtonBox *buttons = new QDialogButtonBox(
					QDialogButtonBox::Ok | QDialogButtonBox::Cancel
					);
			QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
			QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
			mainLayout->addWidget(buttons);
		}

	private:
		static void addTableRow(QTableWidget* table) {
			int row = table->rowCount();
			table->insertRow(row);

			QDateEdit *date = new QDateEdit(QDate::currentDate());
			date->setCalendarPopup(true);
			date->setDisplayFormat("dd/MM/yyyy");
			table->setCellWidget(row, 0, date);

			QComboBox *mold = new QComboBox();
			mold->addItems({"Matriz X", "Matriz Y"});
			table->setCellWidget(row, 1, mold);

			QSpinBox *quantity = new QSpinBox();
			quantity->setMinimum(1);
			quantity->setMaximum(999);
			table->setCellWidget(row, 2, quantity);

			QComboBox *priority = new QComboBox();
			priority->addItems({"Baixa", "Média", "Alta"});
			table->setCellWidget(row, 3, priority);

		}
};
