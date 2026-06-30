#pragma once
#include <QTableWidget>
#include <QMouseEvent>
#include <QHeaderView>
#include "./Table.h"

class TableSimple : public Table {
	Q_OBJECT

	private:
		int hoveredRow = -1;

	public:
		TableSimple(int rows, int cols, QWidget *parent = nullptr)
			: Table(rows, cols, parent)
		{
			setMouseTracking(true);
			setSelectionBehavior(QAbstractItemView::SelectRows);
			setEditTriggers(QAbstractItemView::NoEditTriggers);
			setSelectionMode(QAbstractItemView::NoSelection);
			//horizontalHeader()->setDefaultSectionSize(150);
		}

		void updateData() {
			for (int row = 0; row < 5; row++) {
				for (int col = 0; col < this->columnCount(); col++) {
					this->setItem(row, col, new QTableWidgetItem(""));
				}
			}
			//resizeRowsToContents();
			resizeColumnsToContents();
		}

	protected:
		void mouseMoveEvent(QMouseEvent *event) override {
			int row = rowAt(event->pos().y());
			int col = columnAt(event->pos().x());

			if (row != hoveredRow) {
				updateRowColor(hoveredRow, false);
				hoveredRow = row;
				updateRowColor(hoveredRow, true);
			}

			QTableWidget::mouseMoveEvent(event);
		}
		void leaveEvent(QEvent *event) override {
			updateRowColor(hoveredRow, false);
			hoveredRow = -1;
			QTableWidget::leaveEvent(event);
		}

	private:
		void updateRowColor(int row, bool hovered) {
			if (row < 0 || row >= rowCount()) return;
			for (int col = 0; col < columnCount(); col++) {
				if (auto *item = this->item(row, col)) {
					item->setBackground(hovered ? QColor("#eeeeee") : palette().base());
					item->setForeground(hovered ? QColor("#000000") : palette().text());
				}
			}
		}
};
