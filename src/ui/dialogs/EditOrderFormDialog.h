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
#include <QMessageBox>

class EditOrderFormDialog : public FormDialog {
	Q_OBJECT

	public:
		EditOrderFormDialog(int id, QWidget *parent = nullptr)
			: FormDialog("Editar Pedido")
		{
			auto order = OrderRepository::instance().get_by_id(id);

			addComboInput("Cliente:", [this]() -> std::map<int, std::string> {
					std::map<int, std::string> options;
					for (const auto& c : ClientRepository::instance().all())
					options[c.id().value()] = c.name();
					return options;
					}, [this](std::function<void()> reload) {
					QObject::connect(&ClientRepository::instance(), &ClientRepository::changed, this, [reload]() { reload(); });
					},
					order->client_id()
					);

			addComboInput("Matriz:", [this]() -> std::map<int, std::string> {
					std::map<int, std::string> options;
					for (const auto& m : MoldRepository::instance().all())
					options[m.id().value()] = m.type();
					return options;
					}, [this](std::function<void()> reload) {
					QObject::connect(&MoldRepository::instance(), &MoldRepository::changed, this, [reload]() { reload(); });
					},

					order->mold_id()
					);

			addIntInput("Quantidade:", 1, 999, order->quantity());
			addDateInput("Data de entrega:", QDate(order->delivery_date().date()));
			addComboInput("Prioridade:", [this]() -> std::map<int, std::string> {
					std::map<int, std::string> options;
					options[1] = Priority::text(1);
					options[2] = Priority::text(2);
					options[3] = Priority::text(3);
					return options;
					},
					nullptr,
					order->priority()
					);

			addIntInput("Quantidade Produzida:", 0, 999, order->produced_quantity());

			buildButtons();

			addButton(QIcon(":/icons/trash2.svg"), "Remover", [this, id]() {
					if (!OrderRepository::instance().remove(id)) {
					QMessageBox::warning(this, "Erro", "Não é possível remover este pedido pois ele está sendo referenciado.");
					return;
					}
					close();
					});

			onAccepted([this, id]() {
					auto nOrder = OrderRepository::instance().get_by_id(id);
					nOrder->setClientId(comboValue(0).id);
					nOrder->setMoldId(comboValue(1).id);
					nOrder->setQuantity(intValue(2));
					nOrder->setDeliveryDate(dateValue(3));
					nOrder->setPriority(comboValue(4).id);
					nOrder->setProducedQuantity(intValue(5));
					OrderRepository::instance().update(*nOrder);
					});

		}
};
