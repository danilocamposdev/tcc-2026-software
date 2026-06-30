#include "DatabaseSeeder.h"

void DatabaseSeeder::seed() {
    seed_clients();
    seed_molds();
    seed_orders();
}

bool DatabaseSeeder::is_empty(const QString& table) {
    QSqlQuery query(Database::instance().connection());
    query.exec("SELECT COUNT(*) FROM " + table);
    if (query.next())
        return query.value(0).toInt() == 0;
    return true;
}

void DatabaseSeeder::seed_clients() {
    if (!is_empty("clients")) return;
    QSqlQuery query(Database::instance().connection());
    query.prepare("INSERT INTO clients (name) VALUES (:name)");

    // Mantidos exatamente como na tabela (coluna "Cliente")
    QStringList names = {
        "MIB - MINERAÇÃO IBIRITÉ LTDA",                 // id 1
        "MINERITA - MINERIOS ITAUNA LTDA",               // id 2
        "AURA ALMAS MINERACAO S.A.",                     // id 3
        "DINAMICA MATERIAIS HIDRAULICOS LTDA",           // id 4
        "SAFM",                                          // id 5
        "PEDREIRA UM VALEMIX MICON - MIN CONG",          // id 6
    };

    for (const auto& name : names) {
        query.bindValue(":name", name);
        query.exec();
    }
}

void DatabaseSeeder::seed_molds() {
    if (!is_empty("molds")) return;
    QSqlQuery query(Database::instance().connection());
    query.prepare("INSERT INTO molds (type, available_quantity) VALUES (:type, :available_quantity)");

    // Mantidos exatamente como na tabela (coluna "Matriz")
    QList<QPair<QString, int>> molds = {
        {"CURVA 8\" X 90° LONGO",            2}, // id 1
        {"CURVA 8\" X 90° CURTO",            2}, // id 2
        {"CURVA 8\" X 45° LONGO",            2}, // id 3
        {"CURVA 6\" X 45° LONGO",            2}, // id 4
        {"CURVA 10\" X 45° CURTO",           1}, // id 5
        {"Y DN 14 X 818 MM X 676 MM",        1}, // id 6
        {"REDUÇÃO CONCÊNTRICA 8\" X  6\"",   1}, // id 7
        {"CURVA 6\" X 90° CURTO",            2}, // id 8
        {"CURVA 6\" X 90° LONGO",            2}, // id 9
        {"T DN 8\" X 90°",                   2}, // id 10
        {"CURVA 8\" X 90° LONGO",            2}, // id 11
        {"MANGOTE DN 2\"",                   1}, // id 12
    };

    for (const auto& [type, qty] : molds) {
        query.bindValue(":type", type);
        query.bindValue(":available_quantity", qty);
        query.exec();
    }
}

void DatabaseSeeder::seed_orders() {
    if (!is_empty("orders")) return;
    QSqlQuery query(Database::instance().connection());
    query.prepare(R"(
        INSERT INTO orders (delivery_date, mold_id, quantity, client_id, priority, produced_quantity)
        VALUES (:delivery_date, :mold_id, :quantity, :client_id, :priority, :produced_quantity)
    )");

    // { delivery_date, mold_id, quantity, client_id, priority, produced_quantity }
    QList<std::tuple<QString, int, int, int, int, int>> orders = {
        {"2026-05-27", 1,  2, 1, 2, 0}, // MIB         - CURVA 8" X 90° LONGO
        {"2026-05-05", 2,  2, 2, 2, 0}, // MINERITA    - CURVA 8" X 90° CURTO
        {"2026-05-05", 3,  2, 2, 2, 0}, // MINERITA    - CURVA 8" X 45° LONGO
        {"2026-05-05", 4,  1, 2, 2, 0}, // MINERITA    - CURVA 6" X 45° LONGO
        {"2026-05-05", 5,  1, 2, 2, 0}, // MINERITA    - CURVA 10" X 45° CURTO
        {"2026-06-12", 6,  2, 3, 2, 0}, // AURA ALMAS  - Y DN 14 X 818 MM X 676 MM
        {"2026-05-28", 7,  1, 4, 2, 0}, // DINAMICA    - REDUÇÃO CONCÊNTRICA 8" X 6"
        {"2026-06-03", 8,  1, 2, 2, 0}, // MINERITA    - CURVA 6" X 90° CURTO
        {"2026-06-03", 9,  1, 2, 2, 0}, // MINERITA    - CURVA 6" X 90° LONG
        {"2026-06-03", 10, 1, 2, 2, 0}, // MINERITA    - T DN 8" X 90°
        {"2026-05-19", 11, 1, 5, 2, 0}, // SAFM        - CURVA 8" X 90° LONG
        {"2026-05-21", 2,  3, 2, 2, 0}, // MINERITA    - CURVA 8" X 90° CURTO (mesma matriz id 2)
        {"2026-05-02", 12, 6, 6, 2, 0}, // PEDREIRA UM - MANGOTE DN 2"
    };

    for (const auto& [date, mold_id, qty, client_id, priority, produced_qty] : orders) {
        query.bindValue(":delivery_date", date);
        query.bindValue(":mold_id", mold_id);
        query.bindValue(":quantity", qty);
        query.bindValue(":client_id", client_id);
        query.bindValue(":priority", priority);
        query.bindValue(":produced_quantity", produced_qty);
        query.exec();
    }
}
