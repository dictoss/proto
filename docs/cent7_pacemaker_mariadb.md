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

- 本家redhatの情報
  - ​https://access.redhat.com/documentation/ja-JP/Red_Hat_Enterprise_Linux/7/html/High_Availability_Add-On_Reference/ch-clusteradmin-HAAR.html#s1-clustercreate-HAAR

- ocfファイル
  - /lib/ocf/resource.d/ 配下にある模様
  - /lib/ocf/resource.d/heartbeat/IPaddr2 の存在を確認
    - 名前空間は、pcf:heartbeat:IPaddr2と指定する
  - /lib/ocf/resource.d/heartbeat/mysql の存在を確認
    - 名前空間は、pcf:heartbeat:mysqlと指定する

- サーバ間の認証
  - haclusterユーザを作ってそのUNIXユーザのパスワードを認証する
  - # pcs cluster auth {host1} {host2} -u hacluster -p {passwd}

- リソース設定一覧確認
  - # pcs resource list
  - # pcs resource standard
  - # pcs resource providers
  - # pcs resource list {string}

- クラスタの生成
  - # pcs cluster setup --name {cluster_name} {node1} {node2}

- リソースのグループの作成
  - # pcs resource group add group_name resource_id [resource_id] ... [resource_id] [--before resource_id | --after resource_id

- リソースの追加
  - groupオプションを使うと、グループのリソースへ追加できる。既存グループがない場合はグループも合わせて生成される。

<pre>

# pcs resource create resource_id standard:provider:type|type [resource options]
[op operation_action operation_options [operation_action  operation_options]...]
[meta meta_options...] [--clone clone_options |
--master master_options | --group group_name
[--before  resource_id | --after resource_id]] [--disabled]

</pre>

  - 仮想IP
    - # pcs resource create VirtualIP ocf:heartbeat:IPaddr2 ip=192.168.22.114 cidr_netmask=24 op monitor interval=15s
    - # pcs resource create VirtualIP ocf:heartbeat:IPaddr2 ip=192.168.22.114 cidr_netmask=24 op monitor interval=15s --group MyDbmsCluster

  - mysql
    - # pcs resource create Mysqld ocf:heartbeat:mysql ocf:heartbeat:mysql params binary="/usr/sbin/mysqld" op start timeout="120" op stop timeout="120" op promote timeout="120" op demote timeout="120" op monitor interval=15s role="Master" timeout="30" interval="10" op monitor role="Slave" timeout="30" interval="10"
    - # pcs resource create Mysqld ocf:heartbeat:mysql ocf:heartbeat:mysql params binary="/usr/sbin/mysqld" op start timeout="120" op stop timeout="120" op promote timeout="120" op demote timeout="120" op monitor interval=15s role="Master" timeout="30" interval="10" op monitor role="Slave" timeout="30" interval="10" --group MyDbmsCluster

- リソースの削除
  - # pcs resource delete {resource_id}

- ocfのmysqlの中身を見てみる
  - mysqlの"SHOW SLAVE STATUS"などをみて、slaveなのかmasterなのかのれプリケーション状態を確認する処理がある
  - slaveサーバへpacemakerが起動時に"CHANGE MASTER TO"する仕様の模様
    - レプリケーションはHAクラスタの制御下には入れたくない人もいると思う
    - 余計なことをしてくれる、と思わないでもない
  - "RESET SLAVE;"になっており、"RESET SLAVE ALL;"と実行していない
    - mysql-5.5以降で変わったような気がするので、mysql community editionだとうまく動かないかも。

---

## HAクラスタの構築手順

### 2台のマシンでそれぞれ作業を進めます

- 2台のマシンへCentOS-7をインストールします

- SELinuxを無効にしておきます。（ごめんなさい）

<pre>

# vi /etc/selinux/config
SELINUX=disabled

</pre>

- firewalldを無効化

<pre>

# systemctl disable firewalld
# systemctl stop firewalld

</pre>

- mariadbとpacemakerをインストール

<pre>

# yum install mariadb-server
# yum install pcs corosync pacemaker resource-agents
→なぜかperl、ruby、nanoも入る。

</pre>

- haクラスタのユーザを作成

<pre>
# adduser hacluster
# passwd hacluster
Password: hapass

</pre>

- pacemakerデーモンを起動します

<pre>

# systemctl enable pcsd
# systemctl start pcsd

</pre>


### 2台で協調しながら進めます



---

## フェールオーバーの動作確認


## 切り戻し手順

