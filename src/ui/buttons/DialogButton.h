#pragma once
#include "Button.h"
#include <QDialog>

template<typename TDialog>
class DialogButton : public Button {
	private:
		TDialog mDialog;

	public:
		DialogButton(const QIcon &icon, const QString &label, QWidget *parent = nullptr)
			: Button(icon, label, parent), mDialog(parent)
		{
			QObject::connect(this, &QPushButton::clicked, [this]() {
					mDialog.exec();
					});
		}

		TDialog* dialog() { return &mDialog; }
};
