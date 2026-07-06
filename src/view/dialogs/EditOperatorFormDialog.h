#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"
#include <repository/OperatorRepository.h>
#include <QMessageBox>

class EditOperatorFormDialog : public FormDialog {
	Q_OBJECT

	public:
		EditOperatorFormDialog(int id, QWidget *parent = nullptr)
			: FormDialog("Editar Operador")
		{
			auto op = OperatorRepository::instance().get_by_id(id);

			addTextInput("Nome:", op->name());
			addIntInput("Nível:", 1, 3, op->level());
			buildButtons();

			addButton(QIcon(":/icons/trash2.svg"), "Remover", [this, id]() {
					if (!OperatorRepository::instance().remove(id)) {
					QMessageBox::warning(this, "Erro", "Não é possível remover este operador pois ele está sendo referenciado.");
					return;
					}
					close();
					});
			onAccepted([this, id]() {
					auto nOperator = OperatorRepository::instance().get_by_id(id);
					nOperator->setName(textValue(0));
					nOperator->setLevel(intValue(1));
					OperatorRepository::instance().update(*nOperator);
					});
		}
};
