# mariadb HA Cluster with pacemaker1.1 on CentOS-7

## 目的

- RHEL7からはpacemakerは標準で提供されるようになった
- HAクラスタを構築するheartbeatデーモンは、pacemaker-1.1系にバージョンアップした
- 標準のmariadbをpacemakerを使って冗長する方法を検討する

## 目指す構成

- サーバ1とサーバ2の2台構成とする
- サーバ1にmariadb-master、サーバ2にmariadb-slaveとする
  - slaveは準動機レプリケーションとしたHot-Standby構成とする
- pacemakerのリソース定義
  - 管理するリソース
    - 仮想IP
    - mariadbプロセス
    - mariadbのmaster/slave状態
    - mariadbのslaveのレプリケーションのFILEとPOS
- 切り替え処理の内容
  - 仮想IPを待機系サーバへ移す
  - 昇格するサーバでレプリケーション一時停止・停止、master動作へ遷移するSQLを実行する
- そのほかの動作定義
  - 元々の稼動系を復活後に自動でfailbackしてはならない（nopreemptな動作をさせたい）。  - 切り戻しの手順を確認する。

---

## pacemakerの調査


---

## HAクラスタの構築手順


---

## フェールオーバーの動作確認


## 切り戻し手順

