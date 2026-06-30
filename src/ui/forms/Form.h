if (col == 0) { // Data de entrega
						auto *dateEdit = new QDateEdit();
						dateEdit->setDate(QDate::currentDate());
						dateEdit->setCalendarPopup(true);
						dateEdit->setDisplayFormat("dd/MM/yyyy");
						table->setCellWidget(row, col, dateEdit);
					}
					if (col == 1) {
						auto *combo = new QComboBox();
						combo->addItem("Matriz A");
						combo->addItem("Matriz B");
						combo->addItem("Matriz C");
						table->setCellWidget(row, col, combo);
					}
					if (col == 2) {
						auto *number = new QSpinBox();
						number->setMinimum(1);
						table->setCellWidget(row, col, number);
					}

