# Engine

ElekiEngineメインプロジェクトです。

ESCアーキテクチャを中心に、ゲーム開発基盤ランタイムを構築します。

## 各モジュールプロジェクトのファイル構成

[project]                                 <br>
├── include                               <br>
│      └── [include files]...             <br>
├── source                                <br>
│      └── [source files]...              <br>
├── resource                              <br>
│      └── [resource files]...            <br>
├── library                               <br>
│      └── [library files]...             <br>
└── build                                 <br>
       └── [platform]                     <br>
              └── [configration]          <br>
                     └── [build files]... <br>

includeには公開用ヘッダファイルを置きます。
sourceにはクロスプラットフォームなソースファイルを置きます。
resourceにはアイコンなどのファイルを置きます。
libraryにはこのモジュールが参照する外部ライブラリを置きます。
build内にプラットフォーム、デバッグ、リリース別にビルドしたファイルを出力させます。
テストプロジェクト名は'Test'とします。
ビルドプロジェクト名はアッパーキャメルケースで命名します。

***
© 2022 Taichi Ito
