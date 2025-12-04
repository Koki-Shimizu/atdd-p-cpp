#ifndef PARKING_LOT_HPP
#define PARKING_LOT_HPP

// 料金設定構造体
struct ParkingRateConfig {
    int unitMinutes;      // 料金単位の分数
    int unitPrice;        // 単位料金
    int maxMinutes;       // 最大料金が適用される時間
    int maxFee;           // 最大料金
    
    // 夜間料金設定
    int nightUnitMinutes; // 夜間の料金単位の分数
    int nightUnitPrice;   // 夜間の単位料金
    int nightMaxMinutes;  // 夜間の最大料金が適用される時間
    int nightMaxFee;      // 夜間の最大料金
};

// 基底クラス
class ParkingLot {
public:
    // 後方互換性のためのコンストラクタ（既存のテスト用）
    ParkingLot(int unitMinutes, int unitPrice);
    virtual ~ParkingLot() = default;
    
    virtual int calculateFee(int minutes);
    // 平日と休日が混在する場合の料金計算
    static int calculateFee(int weekdayMinutes, int holidayMinutes, ParkingLot* weekdayLot, ParkingLot* holidayLot);
    // 時刻を指定した料金計算（hour: 0-23, minute: 0-59）
    int calculateFee(int minutes, int startHour, int startMinute);

protected:
    ParkingLot(); // 派生クラス用の保護コンストラクタ
    int unitMinutes_;  // 料金単位の分数（平日: 60分、休日: 30分）
    int unitPrice_;    // 単位料金（500円）
    int maxMinutes_;   // 最大料金が適用される時間（平日: 720分、休日: 360分）
    int maxFee_;       // 最大料金（1500円）
    
    // 夜間料金用のパラメータ
    int nightUnitMinutes_;  // 夜間の料金単位の分数（60分）
    int nightUnitPrice_;    // 夜間の単位料金（300円）
    int nightMaxMinutes_;   // 夜間の最大料金が適用される時間
    int nightMaxFee_;       // 夜間の最大料金（1000円）
    
    int calculateBaseFee(int minutes);
    bool isDaytime(int hour, int minute); // 日中かどうかを判定（08:00-18:00）
    int calculateDaytimeFee(int minutes); // 日中料金を計算
    int calculateNighttimeFee(int minutes); // 夜間料金を計算

private:
    int calculateFeeInternal(int minutes);
};

// 平日クラス
class WeekdayParkingLot : public ParkingLot {
public:
    // デフォルトコンストラクタ（後方互換性のため）
    WeekdayParkingLot();
    // 料金設定を指定するコンストラクタ
    WeekdayParkingLot(const ParkingRateConfig& config);
    // 後方互換性のため、基底クラスのメソッドをオーバーライド
    int calculateFee(int minutes) override;
    // 基底クラスの時刻指定メソッドを使用
    using ParkingLot::calculateFee;
};

// 休日クラス
class HolidayParkingLot : public ParkingLot {
public:
    // デフォルトコンストラクタ（後方互換性のため）
    HolidayParkingLot();
    // 料金設定を指定するコンストラクタ
    HolidayParkingLot(const ParkingRateConfig& config);
    // 後方互換性のため、基底クラスのメソッドをオーバーライド
    int calculateFee(int minutes) override;
    // 基底クラスの時刻指定メソッドを使用
    using ParkingLot::calculateFee;
};

#endif // PARKING_LOT_HPP
