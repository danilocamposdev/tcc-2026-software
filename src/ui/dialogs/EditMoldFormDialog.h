#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"
#include <repository/MoldRepository.h>
#include <QMessageBox>

class EditMoldFormDialog : public FormDialog {
	Q_OBJECT

	public:
		EditMoldFormDialog(int id, QWidget *parent = nullptr)
			: FormDialog("Editar Matriz")
		{
			auto mold = MoldRepository::instance().get_by_id(id);

			addTextInput("Tipo:", mold->type());
			addIntInput("Quantidade disponível:", 0, 999, mold->available_quantity());
			buildButtons();

			addButton(QIcon(":/icons/trash2.svg"), "Remover", [this, id]() {
					if (!MoldRepository::instance().remove(id)) {
					QMessageBox::warning(this, "Erro", "Não é possível remover esta matriz pois ela está sendo referenciado.");
					return;
					}
					close();
					});
			onAccepted([this, id]() {
					auto nMold = MoldRepository::instance().get_by_id(id);
					nMold->setType(textValue(0));
					nMold->setQuantity(intValue(1));
					MoldRepository::instance().update(*nMold);
					});
		}
};
