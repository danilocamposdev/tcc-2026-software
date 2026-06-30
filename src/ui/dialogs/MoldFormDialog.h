#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"
#include <repository/MoldRepository.h>

class MoldFormDialog : public FormDialog {
	Q_OBJECT

	public:
		MoldFormDialog(QWidget *parent = nullptr)
			: FormDialog("Adicionar Matriz")
		{

			addTextInput("Tipo:","");
			addIntInput("Quantidade disponível:", 0, 999, 1);
			buildButtons();

			onAccepted([this]() {
					MoldRepository::instance().save(Mold{textValue(0), intValue(1)});
					});
		}
};
