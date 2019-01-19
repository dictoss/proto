# tokyodebian-170 Bub Squash Party 作業結果

## python-ws4py

### debian packageの調査

- 前回の調査

  - upstreamではpython-3.7でいろいろ壊れているみたい
    - https://github.com/Lawouach/WebSocket-for-Python
    - ここで直せるものは直す必要がある
    - 現在は中断したプロジェクト扱い

  - 最低限のパッチを出すか、websocket2に乗り換えるか
    - websocketsは、python3.4 or higherでのみ動作する

- 今回の作業

  - transition freezeが始まったので、ws4pyのバージョンを上げるのも無理そう。
  - 方針をws4py-0.4.2のunstableのまま、python-3.7で動かす方針として調査。
  - すでにパッチが出ている。ubuntuでマージしているパッチのようだ。

    - https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=903529

  - patchを取り込む前のパッケージのビルドはFailする（python-ws4py (0.4.2+dfsg1-2) ）

    - unittestがfailする。upstreamのとおり、asyncが予約語になったため。 

  - patchを取り込んで「debuild -uc -us」でパッケージをビルド。ビルド成功。このとき、unittestもsuccess。

  - できたdebパッケージをインストール。python3-ws4py_0.4.2+dfsg1-2.1_all.deb

  - nodejsのwebsocketサーバにconnectできて、messageのsend、recvができた。
  - bugreportにテストしたことを書いた

    - https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=903529#19 

## python-cookies FTBFS with python 3.7 as supported version

- https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=912183

- 調べたところ、python3.7から処理に使うRFCを変えたらしい

  - https://docs.python.org/3.7/library/urllib.parse.html#urllib.parse.quote

  - Changed in version 3.7: Moved from RFC 2396 to RFC 3986 for quoting URL strings. “~” is now included in the set of reserved characters.

- テストのコードを書き換えると、とりあえずテストは通ることを確認した。

  - bug報告してみた

  - https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=912183#10

- debian/patchesの作成やupstreamへの報告はまだ。
