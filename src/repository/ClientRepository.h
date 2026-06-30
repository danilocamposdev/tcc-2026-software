#pragma once
#include <QObject>
#include <repository/IClientRepository.h>

class ClientRepository : public QObject, public IClientRepository {
	Q_OBJECT
	public:
		static ClientRepository& instance();
		//explicit ClientRepository(QObject *parent = nullptr) : QObject(parent) {}

		Client save(Client client) override;
		Client update(Client client);
		bool remove(int id);
		std::optional<Client> get_by_id(int id) const override;
		std::vector<Client> all() const override;
	private:
		ClientRepository() : QObject(nullptr) {}
		ClientRepository(const ClientRepository&) = delete;
		ClientRepository& operator=(const ClientRepository&) = delete;

signals:
		void changed();
};
