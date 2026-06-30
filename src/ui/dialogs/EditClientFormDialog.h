#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"
#include <repository/ClientRepository.h>
#include <QMessageBox>

class EditClientFormDialog : public FormDialog {
	Q_OBJECT

	public:
		EditClientFormDialog(int id, QWidget *parent = nullptr)
			: FormDialog("Editar Cliente")
		{
			addTextInput("Nome:", ClientRepository::instance().get_by_id(id)->name());
			buildButtons();

			addButton(QIcon(":/icons/trash2.svg"), "Remover", [this, id]() {
					if (!ClientRepository::instance().remove(id)) {
					QMessageBox::warning(this, "Erro", "Não é possível remover este cliente pois ele está sendo referenciado.");
					return;
					}
					close();
					});

			onAccepted([this, id]() {
					auto client = ClientRepository::instance().get_by_id(id);
					client->setName(textValue(0));
					ClientRepository::instance().update(*client);
					});

		}
};
