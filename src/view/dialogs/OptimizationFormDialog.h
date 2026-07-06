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

			if (config.has_value()) {
				addDateInput("Data de início:", QDate{config->start_date().date()});
				addIntInput("Períodos (dias):", 1, 365, config->horizon());
			} else {
				addDateInput("Data de início:");
				addIntInput("Períodos (dias):", 1, 365);
			}

			buildButtons("Gerar");
		}
};
