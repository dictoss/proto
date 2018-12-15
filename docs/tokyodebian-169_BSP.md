# tokyodebian-169 Bub Squash Party 作業結果

## busterへのアップグレード

- Thinkpad X250でテスト
- debian 9.6をインストール中
  - xfce4をインストールしたX Window System環境
  - 日本語入力はuim + anthy
- /etc/apt/sources.list を busterに書き換え
- apt-get update
- apt-get upgrade
- apt-get dist-upgrade
- reboot
- 起動することを確認。OK。
- apt-get autoremove
- busterにupgrade完了

## python-ws4py

### debian packageの調査

- dpkg -l | grep ws4py
ii  python-ws4py                          0.4.2+dfsg1-2                         all          WebSocket library (Python 2)

- upstreamは、0.5.1をリリースしている
  - https://pypi.org/project/ws4py/0.5.1/

- upstreamに追随してほしいとbugreport
  - https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=916495

## upstreamの調査

- upstreamではpython-3.7でいろいろ壊れているみたい
  - https://github.com/Lawouach/WebSocket-for-Python
  - ここで直せるものは直す必要がある
  - 現在は中断したプロジェクト扱い

- 最低限のパッチを出すか、websocket2に乗り換えるか
  - websocketsは、python3.4 or higherでのみ動作する

- ws4py-0.5.1 $ python2.7 setup.py test
  - all pass

- ws4py-0.5.1 $ python3.7 setup.py test

<pre>
File "/home/username/tmp/ws4py-0.5.1/ws4py/server/tulipserver.py", line 43
  f = asyncio.async(self.handle_initial_handshake())
</pre>

  - 原因調査
    - asyncio.async()の関数名のasyncがpython3.7から予約語と重複するためエラーになる
    - asyncio.async()はpython-3.4.3まで利用可能。python-3.4.4からは非推奨なaliasになっている
      - https://docs.python.jp/3/library/asyncio-task.html#asyncio.ensure_future
    - 以下を対応すればよいと思われる。
      - python-3.4.4 < では、asyncio.async()を呼び出すこと
      - python-3.4.4 <= では asyncio.ensure_future()を呼び出すこと

- https://docs.python.jp/3/library/asyncio-task.html#asyncio.ensure_future
  - とりあえず、修正版は作った。
  - 古いバージョンで動かなくなるので、互換性があるやつを作った
  - すでに出ているパッチでは、python-3.4.4より古いと動かなくなる。

- TravelCIのunittestがpython-3.3で失敗する理由は、tornado-5系がpython3.4+に仕様変更したから
  - これのリクエストを先に出す必要がある。
  - requirements.txtの修正を出したが、書き方が間違ったはTravel CIでエラーになった。
  - リクエストを撤回した。
  - 続きはまた今度。


## 自分のアプリがpython3.7で動くか確認

- https://github.com/dictoss/active-task-summary/

- django-11 LTS系の場合、django-1.11.17まで上げないとpython3.7で動かない。
  - requirements.txtを修正
  - https://github.com/dictoss/active-task-summary/issues/17

## usb-modeswitch

- usb-modeswitchのバージョン
  - upstreamを確認
    - http://www.draisberghof.de/usb_modeswitch/#download
    - 2.5.2で最新版
    - OK

- usb-modeswitch-data 20170806-2の中身
  - NTTドコモから出ているUSBモデム情報
    - L-02C 1004:61dd
    - L-03D 1004:6327
    - L-03F 見つからない
      - config sample
        - https://github.com/dictoss/utils/blob/master/conf/ppp/L03F.conf
  - upstreamを確認
    - http://www.draisberghof.de/usb_modeswitch/#download
    ^ 20170806-2が最新を判明
  - L-03Fを追加するなら、upstreamに入れないといけない
  - upstreamはどこ？
    - ここじゃないみたい https://github.com/digidietze/usb-modeswitch-data
