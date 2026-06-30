#pragma once
#include <repository/IOrderRepository.h>
#include <QObject>

class OrderRepository :  public QObject, public IOrderRepository {
	Q_OBJECT;

	public:
	static OrderRepository& instance();
	//explicit OrderRepository(QObject *parent = nullptr) : QObject(parent) {}

	Order save(Order order) override;
	Order update(Order order);
	bool remove(int id);
	std::optional<Order> get_by_id(int id) const override;
	std::vector<Order> all() const override;

	private:
	OrderRepository() : QObject(nullptr) {}
	OrderRepository(const OrderRepository&) = delete;
	OrderRepository& operator=(const OrderRepository&) = delete;


signals:
	void changed();

};
