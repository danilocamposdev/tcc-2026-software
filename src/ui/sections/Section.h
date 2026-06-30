#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>

class Section : public QWidget {
	Q_OBJECT

	private:
		QIcon mIcon;
		QString mTitle;
		QVBoxLayout *mSectionLayout;
		QHBoxLayout *mHeader;

	public:
		Section(const QIcon icon, const QString &title, QWidget *parent = nullptr)
			: QWidget(parent)
		{
			mIcon = icon;
			mTitle = title;
			mSectionLayout = new QVBoxLayout(this);

			mHeader = new QHBoxLayout();
			QHBoxLayout* headerTitle = new QHBoxLayout();
			QLabel *iconLabel = new QLabel();
			iconLabel->setPixmap(mIcon.pixmap(QSize(24,24)));
			//headerTitle->addWidget(iconLabel);
			QLabel *titleLabel = new QLabel(mTitle);
			headerTitle->addWidget(titleLabel);
			titleLabel->setObjectName("sectionTitle");
			mHeader->addLayout(headerTitle);
			mHeader->addStretch();
			mSectionLayout->addLayout(mHeader);
		}

		void setContent(QWidget *content) {
			mSectionLayout->addWidget(content);
			//mSectionLayout->setContentsMargins(0, 0, 0, 0);
		}

		void addButton(QPushButton* button) {
			mHeader->addWidget(button);
		}
		
		void addButton(QToolButton* button) {
			mHeader->addWidget(button);
		}


		QIcon icon() {
			return mIcon;
		}

		QString title() {
			return mTitle;
		}
};
