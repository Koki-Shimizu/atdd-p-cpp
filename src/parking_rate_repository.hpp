#ifndef PARKING_RATE_REPOSITORY_HPP
#define PARKING_RATE_REPOSITORY_HPP

#include "parking_lot.hpp"
#include <string>
#include <memory>

// 料金設定の保存・読み込み用のインターフェース
class ParkingRateRepository {
public:
    virtual ~ParkingRateRepository() = default;
    
    // 料金設定を保存
    virtual bool save(const std::string& type, const ParkingRateConfig& config) = 0;
    
    // 料金設定を読み込み
    virtual bool load(const std::string& type, ParkingRateConfig& config) = 0;
    
    // 料金設定が存在するか確認
    virtual bool exists(const std::string& type) = 0;
};

// ファクトリ関数（スマートポインタ版）
std::unique_ptr<ParkingRateRepository> createSQLiteRepository(const std::string& dbPath);

// 後方互換性のための生ポインタ版（非推奨）
ParkingRateRepository* createSQLiteRepositoryRaw(const std::string& dbPath);

#endif // PARKING_RATE_REPOSITORY_HPP

