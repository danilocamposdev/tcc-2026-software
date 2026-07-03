#pragma once
#include <QTableWidget>
#include <QMouseEvent>
#include <QHeaderView>
#include <QScrollBar>

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

	protected:
		void wheelEvent(QWheelEvent *event) override {
			const int pixelsPerStep = 40;
			QPoint delta = event->angleDelta();

			if (!delta.isNull()) {
				horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (delta.x() / 120.0) * pixelsPerStep);
				verticalScrollBar()->setValue(
						verticalScrollBar()->value() - (delta.y() / 120.0) * pixelsPerStep);
				event->accept();
			}
		}
};
