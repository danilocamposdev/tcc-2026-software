#pragma once
#include <QSqlDatabase>
#include <QString>

class Database {
public:
    static Database& instance();
    QSqlDatabase& connection();
private:
    Database();
    QSqlDatabase db_;
};
