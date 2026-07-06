#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"
#include <repository/OperatorRepository.h>

class OperatorFormDialog : public FormDialog {
	Q_OBJECT

	public:
		OperatorFormDialog(QWidget *parent = nullptr)
			: FormDialog("Adicionar Operador")
		{

			addTextInput("Nome:","");
			addIntInput("Nível:", 1, 3, 1);
			buildButtons();

			onAccepted([this]() {
					OperatorRepository::instance().save(Operator{textValue(0), intValue(1)});
					});
		}
};
