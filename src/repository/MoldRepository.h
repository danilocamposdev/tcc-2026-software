#pragma once
#include <QObject>
#include <repository/IMoldRepository.h>

class MoldRepository : public QObject, public IMoldRepository {
	Q_OBJECT
	public:
		static MoldRepository& instance();
		//explicit MoldRepository(QObject *parent = nullptr) : QObject(parent) {}

		Mold save(Mold mold) override;
		Mold update(Mold mold);
		bool remove(int id);
		std::optional<Mold> get_by_id(int id) const override;
		std::vector<Mold> all() const override;

	private:
		MoldRepository() : QObject(nullptr) {}
		MoldRepository(const MoldRepository&) = delete;
		MoldRepository& operator=(const MoldRepository&) = delete;

signals:
		void changed();
};
