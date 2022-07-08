# Engine

ElekiEngineメインプロジェクトです。

ESCアーキテクチャを中心に、ゲーム開発基盤ランタイムを構築します。

- 使用言語 C++17
- 推奨コンパイラ VisualC++

## ディレクトリ構成

<pre>
Engine                                                                        <br>
├── binary                                                                   <br>
│   ├── [platform]　       '対象プラットフォームのディレクトリです'              <br>
│   │   ├── [configration] 'Release/Debugディレクトリで、バイナリが配置されます' <br>
│   :   :                                                                     <br>
├── [modele]               '各モジュールディレクトリです'                        <br>
:
</pre>

## 各モジュールプロジェクトのディレクトリ構成

<pre>
[module]                                                          <br>
├── include       '公開ヘッダーファイルを配置します'                 <br>
├── source        'クロスプラットフォームなソースファイルを配置します' <br>
├── resource      'リソースを配置します'                            <br>
├── library       'リンクするライブラリを配置します'                 <br>
└── build         'ビルド用プロジェクトディレクトリです'             <br>
    ├── [project] 'ビルド用プロジェクトのワークスペースです'          <br>
    :
</pre>
***
© 2022 Taichi Ito
