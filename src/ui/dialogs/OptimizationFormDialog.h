#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"
#include "../../repository/OptimizationConfigRepository.h"

class OptimizationFormDialog : public FormDialog {
	Q_OBJECT

	public:
		OptimizationFormDialog(QWidget *parent = nullptr)
			: FormDialog("Otimizar")
		{
			auto config = OptimizationConfigRepository::instance().current();

				addDateInput("Data de início:");
			if (config.has_value()) {
				addIntInput("Períodos (dias):", 1, 365, config->horizon());
				addIntInput("Capacidade Diária (unidades):", 1, 1000, config->daily_capacity());
			} else {
				addIntInput("Períodos (dias):", 1, 365);
				addIntInput("Capacidade Diária (unidades):", 1, 1000);
			}

			buildButtons("Gerar");
		}
};
