#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"

class ProductionFormDialog : public FormDialog {
	Q_OBJECT

	public:
		ProductionFormDialog(QWidget *parent = nullptr)
			: FormDialog("Adicionar Produção Realizada")
		{
			addDateInput("Data:");
			//addComboInput("Cliente:",{"Cliente A", "Cliente B"});
			//addComboInput("Matriz:",{"Matriz A", "Matriz B"});
			addIntInput("Quantidade:", 1, 999);
			buildButtons();
		}
};
