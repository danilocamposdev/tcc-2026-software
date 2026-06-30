#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QCompleter>
#include "../buttons/Button.h"
#include <functional>
#include <model/Date.h>
#include <QPushButton>

class FormDialog : public QDialog {
	Q_OBJECT;
	QFormLayout *mForm;
	QDialogButtonBox *mButtons = nullptr;

	public:
	FormDialog(QString title, QWidget *parent = nullptr)
		: QDialog(parent)
	{
		setWindowTitle(title);
		setMinimumWidth(400);
		mForm = new QFormLayout(this);
	}

	struct ComboSelection {
		int id;
		std::string text;
	};

	void buildButtons(const QString accept = "Salvar", const QString reject = "Cancelar") {
		mButtons = new QDialogButtonBox();
		QPushButton *acceptBtn = mButtons->addButton(accept, QDialogButtonBox::AcceptRole);
		QPushButton *rejectBtn = mButtons->addButton(reject, QDialogButtonBox::RejectRole);
		QObject::connect(mButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
		QObject::connect(mButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
		acceptBtn->setIcon(QIcon(":/icons/check.svg"));
		rejectBtn->setIcon(QIcon(":/icons/x.svg"));
		mForm->addRow(mButtons);
		adjustSize();
	}

	void addButton(const QIcon& icon, const QString& label, std::function<void()> fn) {
		QPushButton *btn = mButtons->addButton(label, QDialogButtonBox::ActionRole);
		btn->setIcon(icon);
		QObject::connect(btn, &Button::clicked, this, [fn]() { fn(); });
	}

	void onAccepted(std::function<void()> fn) {
		QObject::connect(this, &QDialog::accepted, this, [fn]() {
				fn();
				});
	}

	void addTextInput(const QString title, const std::string& initial = "") {
		auto *edit = new QLineEdit();
		if (!initial.empty())
			edit->setText(QString::fromStdString(initial));
		mForm->addRow(title, edit);
	}

	void addComboInput(
			const QString title,
			std::function<std::map<int, std::string>()> optionsFn,
			std::function<void(std::function<void()>)> connectFn = nullptr,
			int initial_id = -1)
	{
		QComboBox *combo = new QComboBox();
		auto reload = [combo, optionsFn, initial_id]() {
			auto options = optionsFn();
			combo->clear();
			QStringList labels;
			for (const auto& [id, text] : options) {
				combo->addItem(QString::fromStdString(text), id);
				labels.append(QString::fromStdString(text));
			}
			QCompleter *completer = new QCompleter(labels, combo);
			completer->setCaseSensitivity(Qt::CaseInsensitive);
			completer->setFilterMode(Qt::MatchContains);
			combo->setCompleter(completer);

			// seleciona o item com o id inicial se fornecido
			if (initial_id != -1) {
				int idx = combo->findData(initial_id);
				combo->setCurrentIndex(idx);
			} else {
				combo->setCurrentIndex(-1);
			}
		};
		reload();
		combo->setEditable(true);
		combo->setInsertPolicy(QComboBox::NoInsert);
		if (connectFn) connectFn(reload);
		mForm->addRow(title, combo);
	}

	void addIntInput(const QString title, int min, int max, int initial = 0) {
		auto *spin = new QSpinBox();
		spin->setMinimum(min);
		spin->setMaximum(max);
		spin->setValue(std::max(min, initial));
		mForm->addRow(title, spin);
	}

	void addDateInput(const QString title, QDate initial = QDate::currentDate()) {
		auto *date = new QDateEdit(initial);
		date->setCalendarPopup(true);
		date->setDisplayFormat("dd/MM/yyyy");
		mForm->addRow(title, date);
	}

	Date dateValue(int index) const {
		auto *edit = qobject_cast<QDateEdit*>(mForm->itemAt(index, QFormLayout::FieldRole)->widget());
		if (!edit) return Date{0, 0, 0};
		QDate d = edit->date();
		return Date{d.year(), static_cast<unsigned int>(d.month()), static_cast<unsigned int>(d.day())};
	}

	int intValue(int index) const {
		auto *spin = qobject_cast<QSpinBox*>(mForm->itemAt(index, QFormLayout::FieldRole)->widget());
		return spin ? spin->value() : 0;
	}

	ComboSelection comboValue(int index) const {
		auto *combo = qobject_cast<QComboBox*>(mForm->itemAt(index, QFormLayout::FieldRole)->widget());
		if (!combo) return {-1, ""};
		return {
			combo->currentData().toInt(),
			combo->currentText().toStdString()
		};
	}

	std::string textValue(int index) const {
		auto *edit = qobject_cast<QLineEdit*>(mForm->itemAt(index, QFormLayout::FieldRole)->widget());
		return edit ? edit->text().toStdString() : "";
	}
};
