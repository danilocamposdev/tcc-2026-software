#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "./JobCardWidget.h"
#include "../../../viewModel/OptimizationTableViewModel.h"

class OptimizationCellWidget : public QWidget {
	Q_OBJECT

	public:
		OptimizationCellWidget(const std::vector<OptimizationTableViewModel::JobCardData>& jobs, QWidget *parent = nullptr)
			: QWidget(parent)
		{
			auto *layout = new QVBoxLayout(this);
			layout->setContentsMargins(0, 0, 0, 0);
			layout->setSpacing(2);

			layout->addStretch();

			for (const auto& job : jobs) {
				layout->addWidget(new JobCardWidget(job, this));
			}

			layout->addStretch();
		}
};
