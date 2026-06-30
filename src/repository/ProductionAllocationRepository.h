#pragma once
#include <repository/IProductionAllocationRepository.h>
#include <QObject>

class ProductionAllocationRepository : public QObject, public IProductionAllocationRepository {
    Q_OBJECT
public:
    ProductionAllocation save(ProductionAllocation allocation) override;
    std::vector<ProductionAllocation> all() const override;
    std::vector<ProductionAllocation> by_order(int order_id) const override;
    void clear() override;

signals:
    void changed();
};
