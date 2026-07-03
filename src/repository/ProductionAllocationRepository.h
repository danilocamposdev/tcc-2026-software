#pragma once
#include <repository/IProductionAllocationRepository.h>
#include <QObject>

class ProductionAllocationRepository : public QObject, public IProductionAllocationRepository {
	Q_OBJECT
	public:
		static ProductionAllocationRepository& instance();
		ProductionAllocation save(ProductionAllocation allocation) override;
		std::vector<ProductionAllocation> all() const override;
		std::vector<ProductionAllocation> by_order(int order_id) const override;
		void clear() override;

	private:
		ProductionAllocationRepository() : QObject(nullptr) {}
		ProductionAllocationRepository(const ProductionAllocationRepository&) = delete;
		ProductionAllocationRepository& operator=(const ProductionAllocationRepository&) = delete;

signals:
		void changed();
};
