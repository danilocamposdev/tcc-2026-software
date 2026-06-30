#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include "./FormDialog.h"
#include <repository/ClientRepository.h>
#include <repository/OrderRepository.h>
#include <repository/MoldRepository.h>
#include <model/Order.h>
#include <model/Priority.h>

class OrderFormDialog : public FormDialog {
	Q_OBJECT

	public:
		OrderFormDialog(QWidget *parent = nullptr)
			: FormDialog("Adicionar Matriz")
		{
			addComboInput("Cliente:", [this]() -> std::map<int, std::string> {
					std::map<int, std::string> options;
					for (const auto& c : ClientRepository::instance().all())
					options[c.id().value()] = c.name();
					return options;
					}, [this](std::function<void()> reload) {
					QObject::connect(&ClientRepository::instance(), &ClientRepository::changed, this, [reload]() { reload(); });
					});

			addComboInput("Matriz:", [this]() -> std::map<int, std::string> {
					std::map<int, std::string> options;
					for (const auto& m : MoldRepository::instance().all())
					options[m.id().value()] = m.type();
					return options;
					}, [this](std::function<void()> reload) {
					QObject::connect(&MoldRepository::instance(), &MoldRepository::changed, this, [reload]() { reload(); });
					});

			addIntInput("Quantidade:", 1, 999);
			addDateInput("Data de entrega:");
			addComboInput("Prioridade:", [this]() -> std::map<int, std::string> {
					std::map<int, std::string> options;
					options[1] = Priority::text(1);
					options[2] = Priority::text(2);
					options[3] = Priority::text(3);
					return options;
					});

			addIntInput("Quantidade Produzida:", 0, 999);

			buildButtons();

			onAccepted([this]() {
					OrderRepository::instance().save(Order{dateValue(3), comboValue(1).id, comboValue(0).id, intValue(2), comboValue(4).id, intValue(5)});
					});

		}
};
