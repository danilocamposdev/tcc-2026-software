#pragma once
#include "Database.h"
#include <QSqlQuery>

class DatabaseSeeder {
public:
    static void seed();
private:
    static bool is_empty(const QString& table);
    static void seed_clients();
    static void seed_operators();
    static void seed_molds();
    static void seed_orders();
};
