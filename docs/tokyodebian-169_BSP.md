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

- dpkg -l | grep ws4py
ii  python-ws4py                          0.4.2+dfsg1-2                         all          WebSocket library (Python 2)

- upstreamは、0.5.1をリリースしている
  - https://pypi.org/project/ws4py/0.5.1/

- upstreamに追随してほしいとレポート
  - https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=916495

- upstreamではpython-3.7でいろいろ壊れているみたい
  - https://github.com/Lawouach/WebSocket-for-Python
  - ここで直せるものは直す必要がある

## 自分のアプリがpython3.7で動くか確認

- https://github.com/dictoss/active-task-summary/

- django-11 LTS系の場合、django-1.11.17まで上げないとpython3.7で動かない。
  - requirements.txtを修正
  - https://github.com/dictoss/active-task-summary/issues/17
