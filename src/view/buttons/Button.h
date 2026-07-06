#pragma once
#include <QPushButton>
#include <QFontMetrics>
#include <QApplication>

class Button : public QPushButton {
	public:
		Button(const QIcon &icon, const QString &label, QWidget *parent = nullptr)
			: QPushButton(icon, label, parent)
		{
			int em = QFontMetrics(qApp->font()).height();
			setIconSize(QSize(0.8* em, 0.8* em));
		}
};
