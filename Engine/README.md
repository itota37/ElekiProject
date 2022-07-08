# Engine

ElekiEngineメインプロジェクトです。

ESCアーキテクチャを中心に、ゲーム開発基盤ランタイムを構築します。

## 各モジュールプロジェクトのファイル構成

<pre>
[module]                                                                                 <br>
├── include '公開ヘッダーファイルを配置します'                                              <br>
├── source 'クロスプラットフォームなソースファイルを配置します'                               <br>
├── resource 'リソースを配置します'                                                        <br>
├── library 'リンクするライブラリを配置します'                                              <br>
└── build                                                                                <br>
    ├── [project] 'ビルド用プロジェクトのワークスペースです'                                 <br>
    └── binary                                                                           <br>
        └── [platform] 'プラットフォーム別にフォルダを用意します'                            <br>
            └── [configration] 'ReleaseまたはDebugフォルダにビルドされたファイルを配置します' <br>
</pre>
***
© 2022 Taichi Ito
