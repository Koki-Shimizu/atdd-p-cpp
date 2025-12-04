# ATDD Practice - C++

C++でのATDD（Acceptance Test-Driven Development）の練習プロジェクトです。

駐車場の料金管理システムを実装しています。

## 機能

- 平日・休日で異なる料金設定
- 日中・夜間で異なる料金設定
- 最大料金の適用
- 料金設定のDB保存・読み込み（SQLite）

## ビルド方法

### 必要な環境
- C++17対応のコンパイラ（g++、clang++など）
- make（通常は標準でインストール済み）
- SQLite3（料金設定のDB機能を使用する場合）

### macOSでのSQLite3のインストール

```bash
# Homebrewを使用する場合
brew install sqlite3
```

### ビルドと実行

```bash
# ビルド
make

# ハローワールドの実行
make run

# テストの実行
make test

# クリーンアップ
make clean
```

## プロジェクト構造

```
.
├── CMakeLists.txt                    # CMakeビルド設定
├── Makefile                          # Makefileビルド設定
├── src/
│   ├── main.cpp                      # メインプログラム
│   ├── parking_lot.hpp               # 駐車場クラスのヘッダー
│   ├── parking_lot.cpp               # 駐車場クラスの実装
│   ├── parking_rate_repository.hpp   # 料金設定リポジトリのヘッダー
│   └── parking_rate_repository.cpp   # 料金設定リポジトリの実装（SQLite）
├── tests/
│   ├── test_main.cpp                 # テストコード
│   └── catch.hpp                     # Catch2テストフレームワーク
└── README.md                         # このファイル
```

## 料金設定

### 平日
- **日中（08:00-18:00）**: 60分500円、最大料金12時間1500円
- **夜間（18:01-07:59）**: 60分300円、最大料金12時間1000円

### 休日
- **日中（08:00-18:00）**: 30分500円、最大料金6時間1500円
- **夜間（18:01-07:59）**: 60分300円、最大料金6時間1000円

## 使用例

### 基本的な使用

```cpp
#include "parking_lot.hpp"

// 平日の駐車場を作成
WeekdayParkingLot weekdayLot;

// 料金計算（日中）
int fee1 = weekdayLot.calculateFee(60, 10, 0);  // 500円

// 料金計算（夜間）
int fee2 = weekdayLot.calculateFee(60, 20, 0);   // 300円
```

### DBから料金設定を読み込む

```cpp
#include "parking_rate_repository.hpp"

// リポジトリを作成
auto repo = createSQLiteRepository("parking.db");

// 料金設定を読み込み
ParkingRateConfig config;
repo->load("weekday", config);

// 読み込んだ設定で駐車場を作成
WeekdayParkingLot parkingLot(config);
```

## ATDDの進め方

1. 受け入れテストを書く（tests/）
2. テストが失敗することを確認
3. 最小限の実装を行う（src/）
4. テストが通ることを確認
5. リファクタリング

## テスト

```bash
make test
```

すべてのテストが通ることを確認できます。

## ライセンス

このプロジェクトはATDDの練習用です。

