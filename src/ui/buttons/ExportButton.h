#pragma once
#include <QPushButton>
#include <QMenu>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTableWidget>
#include <QPrinter>
#include <QPainter>
#include <QFontMetrics>
#include <QVector>
#include <QPair>
#include <algorithm>
#include <QDate>
#include <QLocale>
#include "../buttons/Button.h"

class ExportButton : public Button {
	Q_OBJECT

	private:
		QTableWidget *mTable;
		bool mActivePDF;
		QString mTitle;

	public:
		ExportButton(QString title, bool activePDF = false, QWidget *parent = nullptr)
			: Button(QIcon(":/icons/file-earmark.svg"), "Exportar", parent),
			mActivePDF{activePDF},
			mTitle{title + " - " + QLocale().toString(QDate::currentDate(), "dd 'de' MMMM 'de' yyyy")}
		{
			QMenu *menu = new QMenu();

			auto *csvAction = menu->addAction(QIcon(":/icons/filetype-csv.svg"), "CSV");
			connect(csvAction, &QAction::triggered, this, &ExportButton::exportCsv);

			if(mActivePDF) {
				auto *pdfAction = menu->addAction(QIcon(":/icons/file-earmark-pdf.svg"), "PDF");
				connect(pdfAction, &QAction::triggered, this, &ExportButton::exportPdf);
			}

			setMenu(menu);
		}

		void setTable(QTableWidget *table) {
			mTable = table;
		}

		private slots:
			QString csvEscape(QString cell) {
				if (cell.contains("\n") || cell.contains("\"") || cell.contains(";")) {
					cell.replace("\"", "\"\"");
					cell = "\"" + cell + "\"";
				}
				return cell;
			}

		void exportCsv() {
			if (!mTable) return;

			QString path = QFileDialog::getSaveFileName(
					this, "Salvar CSV", mTitle + ".csv", "CSV (*.csv)");
			if (path.isEmpty()) return;

			QFile file(path);
			if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QMessageBox::warning(this, "Erro", "Não foi possível salvar o arquivo.");
				return;
			}

			QTextStream out(&file);

			// Header
			QStringList headers;
			for (int c = 0; c < mTable->columnCount(); ++c) {
				auto *header = mTable->horizontalHeaderItem(c);
				headers << csvEscape(header ? header->text() : "");
			}
			out << headers.join(";") << "\n";

			// Linhas
			for (int r = 0; r < mTable->rowCount(); ++r) {
				QStringList row;
				for (int c = 0; c < mTable->columnCount(); ++c) {
					auto *item = mTable->item(r, c);
					QString cell = item ? item->text() : "";
					row << csvEscape(cell);
				}
				out << row.join(";") << "\n";
			}

			file.close();
		}

		void exportPdf() {
			if (!mTable || !mActivePDF) return;

			QString path = QFileDialog::getSaveFileName(
					this, "Salvar PDF", mTitle + ".pdf", "PDF (*.pdf)");
			if (path.isEmpty()) return;

			QPrinter printer(QPrinter::HighResolution);
			printer.setOutputFormat(QPrinter::PdfFormat);
			printer.setOutputFileName(path);
			printer.setPageOrientation(QPageLayout::Landscape);
			printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

			QPainter painter(&printer);
			painter.setPen(QPen(Qt::black, 1));

			QRect pageRect = printer.pageRect(QPrinter::DevicePixel).toRect();
			QFontMetrics fm(painter.font());

			int rowCount = mTable->rowCount();
			int colCount = mTable->columnCount();
			int cellPadding = 6;

			// Larguras "naturais" das colunas (só usadas pra decidir o agrupamento por página)
			QVector<int> colWidths(colCount);
			for (int c = 0; c < colCount; ++c)
				colWidths[c] = mTable->columnWidth(c);

			// Altura do cabeçalho
			int headerHeight = 30;
			for (int c = 0; c < colCount; ++c) {
				auto *header = mTable->horizontalHeaderItem(c);
				QString text = header ? header->text() : "";
				QRect bound = fm.boundingRect(
						QRect(0, 0, colWidths[c] - cellPadding, 10000),
						Qt::TextWordWrap | Qt::AlignCenter, text);
				headerHeight = std::max(headerHeight, bound.height() + cellPadding);
			}

			// Altura de cada linha
			QVector<int> rowHeights(rowCount);
			for (int r = 0; r < rowCount; ++r) {
				int maxH = 20;
				for (int c = 0; c < colCount; ++c) {
					auto *item = mTable->item(r, c);
					QString text = item ? item->text() : "";
					QRect bound = fm.boundingRect(
							QRect(0, 0, colWidths[c] - cellPadding, 10000),
							Qt::TextWordWrap, text);
					maxH = std::max(maxH, bound.height() + cellPadding);
				}
				rowHeights[r] = maxH;
			}

			// Blocos de colunas por página (baseado na largura natural, só pra decidir quantas cabem)
			QVector<QPair<int,int>> colPages;
			{
				int start = 0;
				while (start < colCount) {
					int width = 0;
					int end = start;
					while (end < colCount && width + colWidths[end] <= pageRect.width()) {
						width += colWidths[end];
						++end;
					}
					if (end == start) ++end;
					colPages.append({start, end});
					start = end;
				}
			}

			bool firstPage = true;

			for (const auto& colRange : colPages) {
				int colStart = colRange.first;
				int colEnd = colRange.second;

				// Largura real escalada para preencher a página inteira
				int naturalWidth = 0;
				for (int c = colStart; c < colEnd; ++c)
					naturalWidth += colWidths[c];

				double scale = naturalWidth > 0
					? static_cast<double>(pageRect.width()) / naturalWidth
					: 1.0;

				QVector<int> scaledWidths(colEnd - colStart);
				int usedWidth = 0;
				for (int c = colStart; c < colEnd; ++c) {
					int w = static_cast<int>(colWidths[c] * scale);
					scaledWidths[c - colStart] = w;
					usedWidth += w;
				}
				// Ajusta a última coluna pra fechar exatamente na borda da página
				// (evita 1-2px de sobra/falta por causa do arredondamento)
				scaledWidths.last() += (pageRect.width() - usedWidth);

				int rowStart = 0;
				while (rowStart < rowCount) {
					if (!firstPage) printer.newPage();
					firstPage = false;

					int y = pageRect.top();
					int x = pageRect.left();

					// Cabeçalho
					int xHeader = x;
					for (int c = colStart; c < colEnd; ++c) {
						int w = scaledWidths[c - colStart];
						QRect cellRect(xHeader, y, w, headerHeight);
						painter.drawRect(cellRect);
						auto *header = mTable->horizontalHeaderItem(c);
						QString text = header ? header->text() : "";
						painter.drawText(cellRect, Qt::AlignCenter | Qt::TextWordWrap, text);
						xHeader += w;
					}
					y += headerHeight;

					// Linhas
					int rowEnd = rowStart;
					while (rowEnd < rowCount && y + rowHeights[rowEnd] <= pageRect.bottom()) {
						int xCell = x;
						for (int c = colStart; c < colEnd; ++c) {
							int w = scaledWidths[c - colStart];
							QRect cellRect(xCell, y, w, rowHeights[rowEnd]);
							painter.drawRect(cellRect);
							auto *item = mTable->item(rowEnd, c);
							QString text = item ? item->text() : "";
							painter.drawText(cellRect.adjusted(4, 2, -4, -2),
									Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, text);
							xCell += w;
						}
						y += rowHeights[rowEnd];
						++rowEnd;
					}

					if (rowEnd == rowStart) ++rowEnd;
					rowStart = rowEnd;
				}
			}

			painter.end();
		}
};
