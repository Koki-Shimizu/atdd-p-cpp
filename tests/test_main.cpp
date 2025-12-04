#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/parking_lot.hpp"
#include "../src/parking_rate_repository.hpp"
#include <cstdio>
#include <cstring>

TEST_CASE("駐車場料金計算", "[parking]") {
    ParkingLot parkingLot(60, 500); // 60分ごとに500円
    
    SECTION("100分駐車した場合、料金は1000円") {
        int minutes = 100;
        int expectedFee = 1000;
        
        int actualFee = parkingLot.calculateFee(minutes);
        
        REQUIRE(actualFee == expectedFee);
    }
}

TEST_CASE("休日と平日で料金が異なる", "[parking]") {
    SECTION("平日は60分500円（後方互換性）") {
        WeekdayParkingLot parkingLot; // 平日
        
        REQUIRE(parkingLot.calculateFee(60) == 500);
        REQUIRE(parkingLot.calculateFee(100) == 1000);
    }
    
    SECTION("平日の日中料金（08:00-18:00）") {
        WeekdayParkingLot parkingLot;
        
        // 日中60分 → 500円
        REQUIRE(parkingLot.calculateFee(60, 10, 0) == 500); // 10:00開始、60分
        // 日中100分 → 1000円
        REQUIRE(parkingLot.calculateFee(100, 10, 0) == 1000); // 10:00開始、100分
        // 日中12時間 → 最大料金1500円
        REQUIRE(parkingLot.calculateFee(720, 8, 0) == 1500); // 08:00開始、12時間
    }
    
    SECTION("平日の夜間料金（18:01-07:59）") {
        WeekdayParkingLot parkingLot;
        
        // 夜間60分 → 300円
        REQUIRE(parkingLot.calculateFee(60, 20, 0) == 300); // 20:00開始、60分
        // 夜間100分 → 600円（2単位）
        REQUIRE(parkingLot.calculateFee(100, 20, 0) == 600); // 20:00開始、100分
        // 夜間12時間 → 最大料金1000円
        REQUIRE(parkingLot.calculateFee(720, 20, 0) == 1000); // 20:00開始、12時間
        // 深夜（01:00開始）60分 → 300円
        REQUIRE(parkingLot.calculateFee(60, 1, 0) == 300); // 01:00開始、60分
    }
    
    SECTION("休日は30分500円（後方互換性）") {
        HolidayParkingLot parkingLot; // 休日
        
        REQUIRE(parkingLot.calculateFee(30) == 500);
        REQUIRE(parkingLot.calculateFee(60) == 1000);
        REQUIRE(parkingLot.calculateFee(100) == 2000);
    }
    
    SECTION("休日の日中料金（08:00-18:00）") {
        HolidayParkingLot parkingLot;
        
        // 日中30分 → 500円
        REQUIRE(parkingLot.calculateFee(30, 10, 0) == 500); // 10:00開始、30分
        // 日中60分 → 1000円
        REQUIRE(parkingLot.calculateFee(60, 10, 0) == 1000); // 10:00開始、60分
        // 日中6時間 → 最大料金1500円
        REQUIRE(parkingLot.calculateFee(360, 8, 0) == 1500); // 08:00開始、6時間
    }
    
    SECTION("休日の夜間料金（18:01-07:59）") {
        HolidayParkingLot parkingLot;
        
        // 夜間60分 → 300円
        REQUIRE(parkingLot.calculateFee(60, 20, 0) == 300); // 20:00開始、60分
        // 夜間120分 → 600円（2単位）
        REQUIRE(parkingLot.calculateFee(120, 20, 0) == 600); // 20:00開始、120分
        // 夜間6時間 → 最大料金1000円
        REQUIRE(parkingLot.calculateFee(360, 20, 0) == 1000); // 20:00開始、6時間
        // 深夜（01:00開始）60分 → 300円
        REQUIRE(parkingLot.calculateFee(60, 1, 0) == 300); // 01:00開始、60分
    }
}

TEST_CASE("最大料金の適用", "[parking]") {
    SECTION("平日は12時間（720分）まで最大料金1500円") {
        WeekdayParkingLot parkingLot; // 平日
        
        // 12時間ちょうど
        REQUIRE(parkingLot.calculateFee(720) == 1500);
        // 12時間を超える場合も最大料金
        REQUIRE(parkingLot.calculateFee(800) == 1500);
        // 12時間未満は通常料金
        REQUIRE(parkingLot.calculateFee(719) == 1500); // 719分 = 12単位 = 6000円だが、最大料金で1500円
        REQUIRE(parkingLot.calculateFee(600) == 1500); // 600分 = 10単位 = 5000円だが、最大料金で1500円
    }
    
    SECTION("休日は6時間（360分）まで最大料金1500円") {
        HolidayParkingLot parkingLot; // 休日
        
        // 6時間ちょうど
        REQUIRE(parkingLot.calculateFee(360) == 1500);
        // 6時間を超える場合も最大料金
        REQUIRE(parkingLot.calculateFee(400) == 1500);
        // 6時間未満でも通常料金が最大料金を超える場合は最大料金を適用
        REQUIRE(parkingLot.calculateFee(359) == 1500); // 359分 = 12単位 = 6000円だが、最大料金で1500円
        REQUIRE(parkingLot.calculateFee(300) == 1500); // 300分 = 10単位 = 5000円だが、最大料金で1500円
        // 6時間未満で通常料金が最大料金未満の場合は通常料金
        REQUIRE(parkingLot.calculateFee(100) == 2000); // 100分 = 4単位 = 2000円（最大料金1500円を超えるが、6時間未満なので通常料金）
        // ただし、6時間ちょうどまたは超える場合は最大料金を適用
        REQUIRE(parkingLot.calculateFee(360) == 1500); // 6時間ちょうど
        REQUIRE(parkingLot.calculateFee(400) == 1500); // 6時間を超える
    }
}

TEST_CASE("閾値の境界値テスト", "[boundary]") {
    SECTION("平日の閾値テスト（720分）") {
        WeekdayParkingLot parkingLot; // 平日
        
        // 719分（12時間未満、最大料金適用の境界）
        REQUIRE(parkingLot.calculateFee(719) == 1500);
        // 720分（12時間ちょうど、最大料金適用）
        REQUIRE(parkingLot.calculateFee(720) == 1500);
        // 721分（12時間を超える、最大料金適用）
        REQUIRE(parkingLot.calculateFee(721) == 1500);
        
        // 600分（10時間、通常料金5000円だが最大料金適用）
        REQUIRE(parkingLot.calculateFee(600) == 1500);
        // 100分（通常料金1000円、最大料金未適用）
        REQUIRE(parkingLot.calculateFee(100) == 1000);
    }
    
    SECTION("休日の閾値テスト（360分と300分）") {
        HolidayParkingLot parkingLot; // 休日
        
        // 299分（300分未満、通常料金5000円だが最大料金未適用？）
        REQUIRE(parkingLot.calculateFee(299) == 5000);
        // 300分（閾値ちょうど、通常料金5000円だが最大料金適用）
        REQUIRE(parkingLot.calculateFee(300) == 1500);
        // 301分（300分を超える、通常料金5000円だが最大料金適用）
        REQUIRE(parkingLot.calculateFee(301) == 1500);
        
        // 359分（6時間未満、通常料金6000円だが最大料金適用）
        REQUIRE(parkingLot.calculateFee(359) == 1500);
        // 360分（6時間ちょうど、最大料金適用）
        REQUIRE(parkingLot.calculateFee(360) == 1500);
        // 361分（6時間を超える、最大料金適用）
        REQUIRE(parkingLot.calculateFee(361) == 1500);
        
        // 100分（通常料金2000円、最大料金未適用）
        REQUIRE(parkingLot.calculateFee(100) == 2000);
        // 150分（通常料金2500円、最大料金未適用）
        REQUIRE(parkingLot.calculateFee(150) == 2500);
    }
}

TEST_CASE("平日の日中・夜間の閾値テスト", "[weekday_boundary]") {
    SECTION("日中の開始時刻の閾値（08:00）") {
        WeekdayParkingLot parkingLot;
        
        // 07:59開始 → 夜間
        REQUIRE(parkingLot.calculateFee(60, 7, 59) == 300);
        // 08:00開始 → 日中
        REQUIRE(parkingLot.calculateFee(60, 8, 0) == 500);
        // 08:01開始 → 日中
        REQUIRE(parkingLot.calculateFee(60, 8, 1) == 500);
    }
    
    SECTION("日中の終了時刻の閾値（18:00）") {
        WeekdayParkingLot parkingLot;
        
        // 17:59開始 → 日中
        REQUIRE(parkingLot.calculateFee(60, 17, 59) == 500);
        // 18:00開始 → 日中（18:00ちょうどは日中）
        REQUIRE(parkingLot.calculateFee(60, 18, 0) == 500);
        // 18:01開始 → 夜間
        REQUIRE(parkingLot.calculateFee(60, 18, 1) == 300);
    }
    
    SECTION("夜間の境界（深夜から朝まで）") {
        WeekdayParkingLot parkingLot;
        
        // 00:00開始 → 夜間
        REQUIRE(parkingLot.calculateFee(60, 0, 0) == 300);
        // 01:00開始 → 夜間
        REQUIRE(parkingLot.calculateFee(60, 1, 0) == 300);
        // 07:59開始 → 夜間
        REQUIRE(parkingLot.calculateFee(60, 7, 59) == 300);
        // 23:59開始 → 夜間
        REQUIRE(parkingLot.calculateFee(60, 23, 59) == 300);
    }
    
    SECTION("日中料金の最大料金閾値（300分、720分）") {
        WeekdayParkingLot parkingLot;
        
        // 299分 → 通常料金（2500円）
        REQUIRE(parkingLot.calculateFee(299, 10, 0) == 2500);
        // 300分 → 最大料金適用（2500円だが最大料金1500円）
        REQUIRE(parkingLot.calculateFee(300, 10, 0) == 1500);
        // 301分 → 最大料金適用
        REQUIRE(parkingLot.calculateFee(301, 10, 0) == 1500);
        // 719分 → 最大料金適用
        REQUIRE(parkingLot.calculateFee(719, 10, 0) == 1500);
        // 720分 → 最大料金適用
        REQUIRE(parkingLot.calculateFee(720, 10, 0) == 1500);
        // 721分 → 最大料金適用
        REQUIRE(parkingLot.calculateFee(721, 10, 0) == 1500);
    }
    
    SECTION("夜間料金の最大料金閾値（300分、720分）") {
        WeekdayParkingLot parkingLot;
        
        // 299分 → 通常料金（1500円）
        REQUIRE(parkingLot.calculateFee(299, 20, 0) == 1500);
        // 300分 → 最大料金適用（1500円だが最大料金1000円）
        REQUIRE(parkingLot.calculateFee(300, 20, 0) == 1000);
        // 301分 → 最大料金適用
        REQUIRE(parkingLot.calculateFee(301, 20, 0) == 1000);
        // 719分 → 最大料金適用
        REQUIRE(parkingLot.calculateFee(719, 20, 0) == 1000);
        // 720分 → 最大料金適用
        REQUIRE(parkingLot.calculateFee(720, 20, 0) == 1000);
        // 721分 → 最大料金適用
        REQUIRE(parkingLot.calculateFee(721, 20, 0) == 1000);
    }
    
    SECTION("時間帯が変わる境界（日中から夜間へ）") {
        WeekdayParkingLot parkingLot;
        
        // 17:00開始、120分 → 17:00-19:00（日中60分 + 夜間60分）
        // ただし、現在の実装では開始時刻のみで判定するため、17:00開始は日中として扱う
        REQUIRE(parkingLot.calculateFee(120, 17, 0) == 1000); // 日中料金で計算
        // 18:00開始、120分 → 18:00-20:00（日中60分 + 夜間60分）
        REQUIRE(parkingLot.calculateFee(120, 18, 0) == 1000); // 日中料金で計算（18:00は日中）
        // 18:01開始、120分 → 18:01-20:01（夜間）
        REQUIRE(parkingLot.calculateFee(120, 18, 1) == 600); // 夜間料金で計算
    }
}

TEST_CASE("平日と休日が混在する場合の料金計算", "[mixed]") {
    SECTION("金曜日12時から土曜日15時まで（27時間）") {
        // 金曜日12時から24時まで = 12時間 = 720分（平日）
        // 土曜日0時から15時まで = 15時間 = 900分（休日）
        // 合計：1620分
        
        WeekdayParkingLot weekdayLot;
        HolidayParkingLot holidayLot;
        
        // 平日部分：720分 → 最大料金1500円
        // 休日部分：900分 → 最大料金1500円
        // 合計：3000円
        int fee = ParkingLot::calculateFee(720, 900, &weekdayLot, &holidayLot);
        REQUIRE(fee == 3000);
    }
}

TEST_CASE("料金設定のDB保存・読み込み", "[database]") {
    SECTION("平日の料金設定を保存・読み込み") {
        // テスト用の一時DBファイル
        const char* testDb = "/tmp/test_parking.db";
        std::remove(testDb); // 既存のファイルを削除
        
        // スマートポインタを使用してメモリリークを防止
        auto repo = createSQLiteRepository(testDb);
        
        // 料金設定を作成
        ParkingRateConfig config;
        config.unitMinutes = 60;
        config.unitPrice = 500;
        config.maxMinutes = 720;
        config.maxFee = 1500;
        config.nightUnitMinutes = 60;
        config.nightUnitPrice = 300;
        config.nightMaxMinutes = 720;
        config.nightMaxFee = 1000;
        
        // 保存
        REQUIRE(repo->save("weekday", config) == true);
        
        // 存在確認
        REQUIRE(repo->exists("weekday") == true);
        REQUIRE(repo->exists("holiday") == false);
        
        // 読み込み
        ParkingRateConfig loadedConfig;
        REQUIRE(repo->load("weekday", loadedConfig) == true);
        REQUIRE(loadedConfig.unitMinutes == 60);
        REQUIRE(loadedConfig.unitPrice == 500);
        REQUIRE(loadedConfig.maxMinutes == 720);
        REQUIRE(loadedConfig.maxFee == 1500);
        REQUIRE(loadedConfig.nightUnitMinutes == 60);
        REQUIRE(loadedConfig.nightUnitPrice == 300);
        REQUIRE(loadedConfig.nightMaxMinutes == 720);
        REQUIRE(loadedConfig.nightMaxFee == 1000);
        
        // 読み込んだ設定で駐車場を作成
        WeekdayParkingLot parkingLot(loadedConfig);
        REQUIRE(parkingLot.calculateFee(60, 10, 0) == 500);
        REQUIRE(parkingLot.calculateFee(60, 20, 0) == 300);
        
        // スマートポインタなので自動的にdeleteされる
        std::remove(testDb); // テスト後に削除
    }
    
    SECTION("休日の料金設定を保存・読み込み") {
        const char* testDb = "/tmp/test_parking2.db";
        std::remove(testDb);
        
        // スマートポインタを使用してメモリリークを防止
        auto repo = createSQLiteRepository(testDb);
        
        ParkingRateConfig config;
        config.unitMinutes = 30;
        config.unitPrice = 500;
        config.maxMinutes = 360;
        config.maxFee = 1500;
        config.nightUnitMinutes = 60;
        config.nightUnitPrice = 300;
        config.nightMaxMinutes = 360;
        config.nightMaxFee = 1000;
        
        REQUIRE(repo->save("holiday", config) == true);
        
        ParkingRateConfig loadedConfig;
        REQUIRE(repo->load("holiday", loadedConfig) == true);
        REQUIRE(loadedConfig.unitMinutes == 30);
        REQUIRE(loadedConfig.maxMinutes == 360);
        
        HolidayParkingLot parkingLot(loadedConfig);
        REQUIRE(parkingLot.calculateFee(30, 10, 0) == 500);
        REQUIRE(parkingLot.calculateFee(60, 20, 0) == 300);
        
        // スマートポインタなので自動的にdeleteされる
        std::remove(testDb);
    }
}

