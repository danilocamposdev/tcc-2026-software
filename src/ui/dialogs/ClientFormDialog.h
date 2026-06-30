#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"
#include <repository/ClientRepository.h>

class ClientFormDialog : public FormDialog {
	Q_OBJECT

	public:
		ClientFormDialog(QWidget *parent = nullptr)
			: FormDialog("Adicionar Cliente")
		{
			addTextInput("Nome:", "");
			buildButtons();

			onAccepted([this]() {
					ClientRepository::instance().save(Client{textValue(0)});
					});

		}
};
