#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "../../../viewModel/OptimizationTableViewModel.h"

class JobCardWidget : public QWidget {
	Q_OBJECT

	public:
		JobCardWidget(const OptimizationTableViewModel::JobCardData& job, QWidget *parent = nullptr)
			: QWidget(parent)
		{
			setAttribute(Qt::WA_StyledBackground, true);
			setProperty("delayed", job.delay > 0);

			auto *layout = new QVBoxLayout(this);
			layout->setSpacing(2);

			auto *moldLabel = new QLabel(QString("Matriz: %1").arg(job.moldType));
			layout->addWidget(moldLabel);

			//layout->addWidget(new QLabel(QString("Cliente: %1").arg(job.clientName)));
			layout->addWidget(new QLabel(QString("Horas estimadas: %1h").arg(job.duration)));
			layout->addWidget(new QLabel(QString("Nível: %1").arg(job.complexity)));
			layout->addWidget(new QLabel(QString("Entrega: %1").arg(job.deliveryDate)));

			if (job.delay > 0) {
				auto *delayLabel = new QLabel(QString("Atrasado: %1 dia(s)").arg(job.delay));
				delayLabel->setObjectName("delayLabel");
				layout->addWidget(delayLabel);
			}
		}
};
