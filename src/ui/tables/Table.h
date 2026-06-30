#pragma once
#include <QTableWidget>
#include <QMouseEvent>
#include <QHeaderView>

class Table : public QTableWidget {
	Q_OBJECT

	private:
		int hoveredRow = -1;

	public:
		Table(int rows, int cols, QWidget *parent = nullptr)
			: QTableWidget(rows, cols, parent)
		{
			setFocusPolicy(Qt::NoFocus);
			setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
			setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

			horizontalHeader()->setObjectName("horizontalHeader");
			verticalHeader()->setObjectName("verticalHeader");
			verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		}
};
