#pragma once
#include <repository/IOptimizationConfigRepository.h>
#include <QObject>

class OptimizationConfigRepository : public QObject, public IOptimizationConfigRepository {
	Q_OBJECT
	public:
		static OptimizationConfigRepository& instance();
		OptimizationConfig save(OptimizationConfig config) override;
		std::optional<OptimizationConfig> current() const override;
		void clear() override;
	private:
		OptimizationConfigRepository() : QObject(nullptr) {}
		OptimizationConfigRepository(const OptimizationConfigRepository&) = delete;
		OptimizationConfigRepository& operator=(const OptimizationConfigRepository&) = delete;
signals:
		void changed();
};
