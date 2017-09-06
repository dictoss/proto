# Django を 5年使ってみて


## 自己紹介

- pythonを使い始めたのは2011年
- 会社の偉い人から「pythonいいよ」と聞いた
- とりあえず使ってみたら結構はまった
- djangoを使うためにpythonを使い始めた


## django-1.3

- 初めて使った
- XML-RPCサーバ、バッチプログラムを開発
- XML-RPCのライブラリがあって、結構楽にできた
  - このときはpython-3への移行で手間がかかることを知らない
- XML-RPCの処理からバッチプログラムを呼び出す機能は結構苦労した
  - バッチプログラムの大量データINSERT、UPDATE、DELETEを処理する
  - トラザクションをマニュアルでBEGIN、COMMITして高速処理した
  - bulk_create()の登場はdjango-1.4を待つことになる


## django-1.4 LTS

- アプリのディレクトリ構造が変わって泣いた
  - django-1.3で作ったアプリを新たにstartproject、startappして移植することになる
- 最初はLTSと言われておらず、あとでLTSになった


## django-1.5

- django-1.4を使える環境ならだいたい動く
- さくっとバージョンアップできていたころが懐かしい
- auth_userテーブルを拡張する方法にAbstractUserを継承する方法が出てくる
  - 昔はauth_userテーブルにOneToOneField(=外部キー連携)するパターンだった


## django-1.6

- python-2.6に対応した最後のバージョン
- python-3.2、3,3に対応したバージョン
- データベースのトランザクション処理が変わった
  - トランザクションのマニュアル処理している箇所は影響を受けそのまま動かない
  - commit_manuallyデコレータがなくなり泣いた
- OS標準のpython2.6でいいや、と思ってつくったプログラムはこのバージョンで打ち止めを食らう
- 古いwebアプリはこのバージョンを軸にしてpython-2.7でも動くように改修するのがよい
- 技術的負債をどう返すか考えるようになる


## django-1.7

- そういえば使ったことない
- python-3.4に対応
- DBスキーマを適用するmanage.py syncdbがdeprecatedになった
- manage.py migrate登場


## django-1.8 LTS

- 機能的にあまり困ったことがなくなりつつあるバージョン
- EoLは少なくとも2018年4月まで。
- python-2.7でも動く
- LTS扱いでpython-3.4に対応しているため、django-1.6からのアップグレード先のターゲットだった
- Postgresqlでhstore型に公式に対応
- mysqlとmariadbでできるスキーマが違うことがあることに気づく
  - datetime と datetime(6)
- htmlレンダリングするテンプレートエンジンを切り替えできるようになる
  - settings.pyの設定が増えてきた
- auth_userテーブルのスキーマ変更より、アップグレードしたアプリでトラブルが起こる
  - migrateしてスキーマを更新する必要あり
- この頃からLTSしか触らなくなったような気がする
  

## django-1.9

- そういえば使ったことない
- Password validation機能が登場
  - settings.pyがまた増える
- unittestがparallel実行できるようになる


## django-1.10

- そういえば使ったことない


## django-1.11 LTS

- 2017年9月現在、最新のリリース
- python-2.7に対応した最後のバージョン
  - EoLは少なくとも2020年4月まで
  - python-2系で頑張っている人はここで打ち止め
  - python-2.7とpython-3.4+の両方で動くようにアプリ改修を頑張るバージョン
- django-1.6時代のプログラムでhtmlレンダリングするrender()へ移行していなかったため、実行時エラーになることに気づき、プログラムを直す
- pytzが依存でインストールされる
  - pytz==2017.2からAsia/Tokyoで"09:19"のずれになる事象に悩まされる


## django-2.0, 2.1, 2.2 LTS

- バージョン命名規則が変わる
- ロードマップ
  - https://www.djangoproject.com/download/
- django-2.0のリリースは、2017年12月の予定
  - python-2.7以下は動作せず
  - python-3.5+でのサポートになる
- django-2.1のリリースは、2018年8月の予定
- django-2.2 LTSのリリースは、2019年4月の予定


## 終わりに

- pythonっていいですよね
- djangoっていいですよね
- フルスタックなwebフレームワークのため、学習は結構大変
- django使ってみてください
