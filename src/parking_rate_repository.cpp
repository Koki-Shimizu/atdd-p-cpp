#include "parking_rate_repository.hpp"
#include <sqlite3.h>
#include <iostream>
#include <cstring>
#include <memory>

class SQLiteParkingRateRepository : public ParkingRateRepository {
private:
    sqlite3* db_;
    std::string dbPath_;
    
    bool initializeDatabase() {
        const char* createTableSQL = 
            "CREATE TABLE IF NOT EXISTS parking_rates ("
            "type TEXT PRIMARY KEY,"
            "unit_minutes INTEGER,"
            "unit_price INTEGER,"
            "max_minutes INTEGER,"
            "max_fee INTEGER,"
            "night_unit_minutes INTEGER,"
            "night_unit_price INTEGER,"
            "night_max_minutes INTEGER,"
            "night_max_fee INTEGER"
            ");";
        
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, createTableSQL, nullptr, nullptr, &errMsg);
        
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        
        return true;
    }
    
public:
    SQLiteParkingRateRepository(const std::string& dbPath) : db_(nullptr), dbPath_(dbPath) {
        int rc = sqlite3_open(dbPath_.c_str(), &db_);
        if (rc != SQLITE_OK) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db_) << std::endl;
            db_ = nullptr;
        } else {
            initializeDatabase();
        }
    }
    
    ~SQLiteParkingRateRepository() {
        if (db_) {
            sqlite3_close(db_);
        }
    }
    
    bool save(const std::string& type, const ParkingRateConfig& config) override {
        if (!db_) return false;
        
        const char* insertSQL = 
            "INSERT OR REPLACE INTO parking_rates "
            "(type, unit_minutes, unit_price, max_minutes, max_fee, "
            "night_unit_minutes, night_unit_price, night_max_minutes, night_max_fee) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, insertSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            return false;
        }
        
        sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, config.unitMinutes);
        sqlite3_bind_int(stmt, 3, config.unitPrice);
        sqlite3_bind_int(stmt, 4, config.maxMinutes);
        sqlite3_bind_int(stmt, 5, config.maxFee);
        sqlite3_bind_int(stmt, 6, config.nightUnitMinutes);
        sqlite3_bind_int(stmt, 7, config.nightUnitPrice);
        sqlite3_bind_int(stmt, 8, config.nightMaxMinutes);
        sqlite3_bind_int(stmt, 9, config.nightMaxFee);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    bool load(const std::string& type, ParkingRateConfig& config) override {
        if (!db_) return false;
        
        const char* selectSQL = 
            "SELECT unit_minutes, unit_price, max_minutes, max_fee, "
            "night_unit_minutes, night_unit_price, night_max_minutes, night_max_fee "
            "FROM parking_rates WHERE type = ?;";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, selectSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            return false;
        }
        
        sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);
        
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            config.unitMinutes = sqlite3_column_int(stmt, 0);
            config.unitPrice = sqlite3_column_int(stmt, 1);
            config.maxMinutes = sqlite3_column_int(stmt, 2);
            config.maxFee = sqlite3_column_int(stmt, 3);
            config.nightUnitMinutes = sqlite3_column_int(stmt, 4);
            config.nightUnitPrice = sqlite3_column_int(stmt, 5);
            config.nightMaxMinutes = sqlite3_column_int(stmt, 6);
            config.nightMaxFee = sqlite3_column_int(stmt, 7);
            sqlite3_finalize(stmt);
            return true;
        }
        
        sqlite3_finalize(stmt);
        return false;
    }
    
    bool exists(const std::string& type) override {
        if (!db_) return false;
        
        const char* selectSQL = "SELECT 1 FROM parking_rates WHERE type = ?;";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, selectSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            return false;
        }
        
        sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        bool exists = (rc == SQLITE_ROW);
        sqlite3_finalize(stmt);
        
        return exists;
    }
};

// ファクトリ関数（スマートポインタ版）
std::unique_ptr<ParkingRateRepository> createSQLiteRepository(const std::string& dbPath) {
    return std::make_unique<SQLiteParkingRateRepository>(dbPath);
}

// 後方互換性のための生ポインタ版（非推奨）
ParkingRateRepository* createSQLiteRepositoryRaw(const std::string& dbPath) {
    return new SQLiteParkingRateRepository(dbPath);
}

