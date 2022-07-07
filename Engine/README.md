# Engine

ElekiEngineメインプロジェクトです。

ESCアーキテクチャを中心に、ゲーム開発基盤ランタイムを構築します。

## 各モジュールプロジェクトのファイル構成

.
├── include <br>
│   └── "file".hpp
├── source
│   └── "file".cpp
├── resource
│   └── "image".png
├── library
│   └── "use_library".lib
└── build
    └── "platform"
        └── "configration"
            ├── module_name.dll
            └── module_name.lib

includeには公開用ヘッダファイルを置きます。
sourceにはクロスプラットフォームなソースファイルを置きます。
resourceにはアイコンなどのファイルを置きます。
libraryにはこのモジュールが参照する外部ライブラリを置きます。
build内にプラットフォーム、デバッグ、リリース別にビルドしたファイルを出力させます。
テストプロジェクト名は'Test'とします。
ビルドプロジェクト名はアッパーキャメルケースで命名します。

***
© 2022 Taichi Ito
