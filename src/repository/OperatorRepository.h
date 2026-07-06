#pragma once
#include <QObject>
#include <repository/IOperatorRepository.h>

class OperatorRepository : public QObject, public IOperatorRepository {
	Q_OBJECT
	public:
		static OperatorRepository& instance();

		Operator save(Operator op) override;
		Operator update(Operator op);
		bool remove(int id);
		std::optional<Operator> get_by_id(int id) const override;
		std::vector<Operator> all() const override;

	private:
		OperatorRepository() : QObject(nullptr) {}
		OperatorRepository(const OperatorRepository&) = delete;
		OperatorRepository& operator=(const OperatorRepository&) = delete;

signals:
		void changed();
};
