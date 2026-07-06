#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QApplication>
#include <QSettings>
#include <QMessageBox>
#include <QSettings>
#include "./FormDialog.h"

class PersonalizeFormDialog : public FormDialog {
	Q_OBJECT

	public:
		PersonalizeFormDialog(QWidget *parent = nullptr)
			: FormDialog("Adicionar Cliente")
		{
			QSettings settings;
			int currentSize = settings.value("fontSize", qApp->font().pointSize()).toInt();

			addIntInput("Tamanho da Fonte", 8, 18, currentSize);
			buildButtons();

			onAccepted([this]() {
					int newSize = intValue(0);

					QFont font = qApp->font();
					font.setPointSize(newSize);
					qApp->setFont(font);

					QSettings settings;
					settings.setValue("fontSize", newSize);

					QMessageBox::information(this, "Reinicialização necessária",
							"Reinicie o aplicativo para que as alterações sejam aplicadas.");
					});

		}
};
