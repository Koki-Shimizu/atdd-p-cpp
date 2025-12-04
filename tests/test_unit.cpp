// ユニットテスト
#include "catch.hpp"
#include "../src/parking_lot.hpp"
#include "../src/parking_rate_repository.hpp"
#include <cstdio>
#include <cstring>

TEST_CASE("ユニットテスト: calculateBaseFee", "[unit]") {
    SECTION("基本料金計算のテスト") {
        ParkingLot lot(60, 500);
        
        // 60分ちょうど = 1単位 = 500円
        REQUIRE(lot.calculateFee(60) == 500);
        
        // 59分 = 1単位（切り上げ）= 500円
        REQUIRE(lot.calculateFee(59) == 500);
        
        // 61分 = 2単位（切り上げ）= 1000円
        REQUIRE(lot.calculateFee(61) == 1000);
        
        // 120分 = 2単位 = 1000円
        REQUIRE(lot.calculateFee(120) == 1000);
        
        // 119分 = 2単位（切り上げ）= 1000円
        REQUIRE(lot.calculateFee(119) == 1000);
    }
    
    SECTION("異なる単位時間でのテスト") {
        // 30分単位、500円
        ParkingLot lot30(30, 500);
        REQUIRE(lot30.calculateFee(30) == 500);
        REQUIRE(lot30.calculateFee(29) == 500);
        REQUIRE(lot30.calculateFee(31) == 1000);
        REQUIRE(lot30.calculateFee(60) == 1000);
        
        // 15分単位、300円
        ParkingLot lot15(15, 300);
        REQUIRE(lot15.calculateFee(15) == 300);
        REQUIRE(lot15.calculateFee(14) == 300);
        REQUIRE(lot15.calculateFee(16) == 600);
        REQUIRE(lot15.calculateFee(45) == 900);
    }
}

TEST_CASE("ユニットテスト: isDaytime", "[unit]") {
    SECTION("日中の時間帯の判定（calculateFeeを通して間接的にテスト）") {
        WeekdayParkingLot lot;
        
        // 日中料金（500円/60分）と夜間料金（300円/60分）の違いで判定
        // 08:00-17:59は日中（500円）
        REQUIRE(lot.calculateFee(60, 8, 0) == 500);
        REQUIRE(lot.calculateFee(60, 8, 1) == 500);
        REQUIRE(lot.calculateFee(60, 12, 0) == 500);
        REQUIRE(lot.calculateFee(60, 17, 59) == 500);
        
        // 18:00ちょうどは日中（500円）
        REQUIRE(lot.calculateFee(60, 18, 0) == 500);
        
        // 18:01以降は夜間（300円）
        REQUIRE(lot.calculateFee(60, 18, 1) == 300);
        REQUIRE(lot.calculateFee(60, 19, 0) == 300);
        REQUIRE(lot.calculateFee(60, 23, 59) == 300);
        
        // 00:00-07:59は夜間（300円）
        REQUIRE(lot.calculateFee(60, 0, 0) == 300);
        REQUIRE(lot.calculateFee(60, 1, 0) == 300);
        REQUIRE(lot.calculateFee(60, 7, 59) == 300);
    }
}

TEST_CASE("ユニットテスト: calculateDaytimeFee", "[unit]") {
    SECTION("平日の日中料金計算") {
        WeekdayParkingLot lot;
        
        // 通常料金
        REQUIRE(lot.calculateFee(60, 10, 0) == 500);
        REQUIRE(lot.calculateFee(100, 10, 0) == 1000);
        REQUIRE(lot.calculateFee(200, 10, 0) == 2000);
        
        // 最大料金が適用されるケース
        REQUIRE(lot.calculateFee(300, 10, 0) == 1500);  // 300分 = 2500円だが最大料金1500円
        REQUIRE(lot.calculateFee(600, 10, 0) == 1500);  // 600分 = 5000円だが最大料金1500円
        REQUIRE(lot.calculateFee(720, 10, 0) == 1500);  // 720分 = 最大料金
        REQUIRE(lot.calculateFee(800, 10, 0) == 1500);  // 800分 = 最大料金
        
        // 最大料金が適用されないケース（100分未満）
        REQUIRE(lot.calculateFee(100, 10, 0) == 1000);  // 100分 = 1000円（最大料金未満）
    }
    
    SECTION("休日の日中料金計算") {
        HolidayParkingLot lot;
        
        // 通常料金
        REQUIRE(lot.calculateFee(30, 10, 0) == 500);
        REQUIRE(lot.calculateFee(60, 10, 0) == 1000);
        REQUIRE(lot.calculateFee(100, 10, 0) == 2000);
        
        // 最大料金が適用されるケース
        REQUIRE(lot.calculateFee(300, 10, 0) == 1500);  // 300分 = 5000円だが最大料金1500円
        REQUIRE(lot.calculateFee(360, 10, 0) == 1500);  // 360分 = 最大料金
        REQUIRE(lot.calculateFee(400, 10, 0) == 1500);  // 400分 = 最大料金
        
        // 最大料金が適用されないケース（100分未満）
        REQUIRE(lot.calculateFee(100, 10, 0) == 2000);  // 100分 = 2000円（最大料金未満）
    }
}

TEST_CASE("ユニットテスト: calculateNighttimeFee", "[unit]") {
    SECTION("平日の夜間料金計算") {
        WeekdayParkingLot lot;
        
        // 通常料金
        REQUIRE(lot.calculateFee(60, 20, 0) == 300);
        REQUIRE(lot.calculateFee(100, 20, 0) == 600);
        REQUIRE(lot.calculateFee(200, 20, 0) == 1200);
        
        // 最大料金が適用されるケース
        REQUIRE(lot.calculateFee(300, 20, 0) == 1000);  // 300分 = 1500円だが最大料金1000円
        REQUIRE(lot.calculateFee(600, 20, 0) == 1000);  // 600分 = 3000円だが最大料金1000円
        REQUIRE(lot.calculateFee(720, 20, 0) == 1000);  // 720分 = 最大料金
        REQUIRE(lot.calculateFee(800, 20, 0) == 1000);  // 800分 = 最大料金
        
        // 最大料金が適用されないケース
        REQUIRE(lot.calculateFee(100, 20, 0) == 600);  // 100分 = 600円（最大料金未満）
    }
    
    SECTION("休日の夜間料金計算") {
        HolidayParkingLot lot;
        
        // 通常料金
        REQUIRE(lot.calculateFee(60, 20, 0) == 300);
        REQUIRE(lot.calculateFee(120, 20, 0) == 600);
        REQUIRE(lot.calculateFee(180, 20, 0) == 900);
        
        // 最大料金が適用されるケース
        REQUIRE(lot.calculateFee(300, 20, 0) == 1000);  // 300分 = 1500円だが最大料金1000円
        REQUIRE(lot.calculateFee(360, 20, 0) == 1000);  // 360分 = 最大料金
        REQUIRE(lot.calculateFee(400, 20, 0) == 1000);  // 400分 = 最大料金
        
        // 最大料金が適用されないケース
        REQUIRE(lot.calculateFee(120, 20, 0) == 600);  // 120分 = 600円（最大料金未満）
    }
}

TEST_CASE("ユニットテスト: ParkingRateRepository", "[unit]") {
    SECTION("保存と読み込みの基本動作") {
        const char* testDb = "/tmp/test_unit_repo.db";
        std::remove(testDb);
        
        auto repo = createSQLiteRepository(testDb);
        
        ParkingRateConfig config;
        config.unitMinutes = 45;
        config.unitPrice = 400;
        config.maxMinutes = 600;
        config.maxFee = 1200;
        config.nightUnitMinutes = 45;
        config.nightUnitPrice = 250;
        config.nightMaxMinutes = 600;
        config.nightMaxFee = 800;
        
        // 保存
        REQUIRE(repo->save("test", config) == true);
        
        // 存在確認
        REQUIRE(repo->exists("test") == true);
        REQUIRE(repo->exists("nonexistent") == false);
        
        // 読み込み
        ParkingRateConfig loaded;
        REQUIRE(repo->load("test", loaded) == true);
        REQUIRE(loaded.unitMinutes == 45);
        REQUIRE(loaded.unitPrice == 400);
        REQUIRE(loaded.maxMinutes == 600);
        REQUIRE(loaded.maxFee == 1200);
        REQUIRE(loaded.nightUnitMinutes == 45);
        REQUIRE(loaded.nightUnitPrice == 250);
        REQUIRE(loaded.nightMaxMinutes == 600);
        REQUIRE(loaded.nightMaxFee == 800);
        
        std::remove(testDb);
    }
    
    SECTION("複数の設定を保存・読み込み") {
        const char* testDb = "/tmp/test_unit_repo2.db";
        std::remove(testDb);
        
        auto repo = createSQLiteRepository(testDb);
        
        ParkingRateConfig config1;
        config1.unitMinutes = 60;
        config1.unitPrice = 500;
        config1.maxMinutes = 720;
        config1.maxFee = 1500;
        config1.nightUnitMinutes = 60;
        config1.nightUnitPrice = 300;
        config1.nightMaxMinutes = 720;
        config1.nightMaxFee = 1000;
        
        ParkingRateConfig config2;
        config2.unitMinutes = 30;
        config2.unitPrice = 500;
        config2.maxMinutes = 360;
        config2.maxFee = 1500;
        config2.nightUnitMinutes = 60;
        config2.nightUnitPrice = 300;
        config2.nightMaxMinutes = 360;
        config2.nightMaxFee = 1000;
        
        // 複数の設定を保存
        REQUIRE(repo->save("weekday", config1) == true);
        REQUIRE(repo->save("holiday", config2) == true);
        
        // それぞれを読み込み
        ParkingRateConfig loaded1, loaded2;
        REQUIRE(repo->load("weekday", loaded1) == true);
        REQUIRE(repo->load("holiday", loaded2) == true);
        
        REQUIRE(loaded1.unitMinutes == 60);
        REQUIRE(loaded2.unitMinutes == 30);
        
        std::remove(testDb);
    }
    
    SECTION("上書き保存のテスト") {
        const char* testDb = "/tmp/test_unit_repo3.db";
        std::remove(testDb);
        
        auto repo = createSQLiteRepository(testDb);
        
        ParkingRateConfig config1;
        config1.unitMinutes = 60;
        config1.unitPrice = 500;
        config1.maxMinutes = 720;
        config1.maxFee = 1500;
        config1.nightUnitMinutes = 60;
        config1.nightUnitPrice = 300;
        config1.nightMaxMinutes = 720;
        config1.nightMaxFee = 1000;
        
        ParkingRateConfig config2;
        config2.unitMinutes = 30;
        config2.unitPrice = 400;
        config2.maxMinutes = 600;
        config2.maxFee = 1200;
        config2.nightUnitMinutes = 45;
        config2.nightUnitPrice = 250;
        config2.nightMaxMinutes = 600;
        config2.nightMaxFee = 800;
        
        // 最初の設定を保存
        repo->save("test", config1);
        
        // 同じキーで上書き保存
        REQUIRE(repo->save("test", config2) == true);
        
        // 上書きされた設定が読み込まれることを確認
        ParkingRateConfig loaded;
        REQUIRE(repo->load("test", loaded) == true);
        REQUIRE(loaded.unitMinutes == 30);  // 上書き後の値
        REQUIRE(loaded.unitPrice == 400);
        
        std::remove(testDb);
    }
}

TEST_CASE("ユニットテスト: エッジケース", "[unit]") {
    SECTION("0分のテスト") {
        ParkingLot lot(60, 500);
        REQUIRE(lot.calculateFee(0) == 0);  // 0分 = 0単位 = 0円
        
        WeekdayParkingLot weekdayLot;
        REQUIRE(weekdayLot.calculateFee(0, 10, 0) == 0);
        REQUIRE(weekdayLot.calculateFee(0, 20, 0) == 0);
    }
    
    SECTION("非常に長い時間のテスト") {
        WeekdayParkingLot lot;
        
        // 24時間 = 1440分
        REQUIRE(lot.calculateFee(1440, 10, 0) == 1500);  // 最大料金
        REQUIRE(lot.calculateFee(1440, 20, 0) == 1000);  // 夜間最大料金
        
        // 48時間 = 2880分
        REQUIRE(lot.calculateFee(2880, 10, 0) == 1500);  // 最大料金
        REQUIRE(lot.calculateFee(2880, 20, 0) == 1000);  // 夜間最大料金
    }
    
    SECTION("境界値のテスト") {
        WeekdayParkingLot lot;
        
        // 299分と300分の境界
        REQUIRE(lot.calculateFee(299, 10, 0) == 2500);  // 通常料金
        REQUIRE(lot.calculateFee(300, 10, 0) == 1500);  // 最大料金適用
        
        // 719分と720分の境界
        REQUIRE(lot.calculateFee(719, 10, 0) == 1500);  // 最大料金
        REQUIRE(lot.calculateFee(720, 10, 0) == 1500);  // 最大料金
        REQUIRE(lot.calculateFee(721, 10, 0) == 1500);  // 最大料金
    }
}

