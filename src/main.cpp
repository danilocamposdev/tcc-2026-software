#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QLabel>
#include <QFile>
#include <QLocale>
#include "./ui/sections/SectionOrders.h"
#include "./ui/sections/SectionMolds.h"
#include "./ui/sections/SectionClients.h"
#include "./ui/sections/SectionOptimization.h"
#include <QPalette>
#include <QStyleFactory>
#include <QMenuBar>
#include "database/Database.h"
#include "database/DatabaseSeeder.h"
#include <repository/OrderRepository.h>
#include <repository/MoldRepository.h>
#include <repository/ClientRepository.h>
#include <repository/OptimizationConfigRepository.h>
#include <repository/ProductionAllocationRepository.h>
#include "./ui/dialogs/PersonalizeFormDialog.h"
#include "./ui/buttons/Button.h"


int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("BarbosaMinerals");
	QCoreApplication::setApplicationName("ProductionOptimization");
	QLocale::setDefault(QLocale(QLocale::Portuguese, QLocale::Brazil));

	Database::instance();
	DatabaseSeeder::seed();
	QPalette palette = app.palette();
	palette.setColor(QPalette::Highlight, QColor("#d0d0d0"));
	palette.setColor(QPalette::HighlightedText, QColor("#000000"));
	app.setPalette(palette);

	QSettings settings;
	QFont font = app.font();
	font.setPointSize(settings.value("fontSize", 14).toInt());
	app.setFont(font);
	int em = QFontMetrics(font).height();

	app.setStyle(QStyleFactory::create("Fusion"));
	QFile styleFile(":/styles/global.qss");
	styleFile.open(QFile::ReadOnly);
	app.setStyleSheet(styleFile.readAll());


	QMainWindow window;
	window.resize(640, 480);
	window.setWindowTitle("Sistema de Alocação de Produção");

	QWidget *central = new QWidget();
	QHBoxLayout *mainLayout = new QHBoxLayout(central);

	QMenuBar *menuBar = window.menuBar();

	QMenu *fileMenu = menuBar->addMenu("Dados");
	fileMenu->addAction("Fazer Backup");
	fileMenu->addAction("Importar Backup");

	QMenu *adjustsMenu = menuBar->addMenu("Ajustes");
	QAction *personalizeAction = adjustsMenu->addAction(QIcon(":/icons/sliders.svg"), "Personalizar");

	auto *personalizeDialog = new PersonalizeFormDialog();
	QObject::connect(personalizeAction, &QAction::triggered, [personalizeDialog]() {
			personalizeDialog->exec();
			});

	QMenu *aboutMenu = menuBar->addMenu("Sobre");
	aboutMenu->addAction("Versão 1.0.0");

	// Menu
	QWidget *menu = new QWidget();
	menu->setObjectName("menu");
	menu->setFixedWidth(13*em);
	QVBoxLayout *menuLayout = new QVBoxLayout(menu);
	menuLayout->setAlignment(Qt::AlignTop);
	menuLayout->setSpacing(1*em);
	QVBoxLayout *menuButtonsLayout = new QVBoxLayout();
	menuButtonsLayout->setSpacing(0.25*em);

	QLabel *logo = new QLabel();
	QPixmap pixmap(":/images/logo.png");
	logo->setPixmap(pixmap.scaledToHeight(5.5*em, Qt::SmoothTransformation));
	logo->setFixedHeight(5.5*em);
	logo->setAlignment(Qt::AlignCenter);
	menuLayout->addWidget(logo);
	menuLayout->addLayout(menuButtonsLayout);

	// Botões do menu
	QButtonGroup *buttonGroup = new QButtonGroup(central);
	buttonGroup->setExclusive(true);

	const QList<Section*> sections = {
		new SectionOptimization(),
		new SectionOrders(),
		new SectionMolds(),
		new SectionClients()
	};

	// Stack de seções
	QStackedWidget *stack = new QStackedWidget();

	Button *firstActive = nullptr;
	for (int index = 0; index < sections.size(); ++index) {
		Section *section = sections[index];
		stack->addWidget(section);
		auto *button = new Button(QIcon(section->icon()), section->title());
		button->setObjectName("menuButton");
		button->setFlat(true);
		button->setCheckable(index >= 0);
		menuButtonsLayout->addWidget(button);

		if (index >= 0) {
			buttonGroup->addButton(button);
			if (!firstActive) firstActive = button;
			QObject::connect(button, &QPushButton::clicked, [stack, index]() {
					stack->setCurrentIndex(index);
					});
		}
	}

	// Marca o primeiro botão ativo como selecionado
	if (firstActive) firstActive->setChecked(true);

	mainLayout->addWidget(menu);
	mainLayout->addWidget(stack, 1);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	window.setCentralWidget(central);
	window.show();

	return app.exec();
}
