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

### 2台のマシンでパッケージのインストール作業を進め、下準備をします

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


### 2台のサーバ両方でmariadbの設定を行います

- 起動するか確認します

<pre>

# systemctl start mariadb
# systemctl stop mariadb

</pre>

- my.cnfを設定します。
  - 1号機(server-id=101)

<pre>

# vi /etc/my.cnf.d/server.cnf

[mysqld]
server-id=101
character-set-server=utf8
log-bin=aria_log


# systemctl start mariadb

</pre>

  - 2号機(server-id=102)

<pre>

# vi /etc/my.cnf.d/server.cnf

[mysqld]
server-id=102
character-set-server=utf8
log-bin=aria_log

# systemctl start mariadb

</pre>

- mysqlの初期設定と権限設定をします。
  - 1号機、2号機ともに設定します

<pre>

# mysql_secure_installation

→rootパスワードを設定してください。

# mysql -u root -p
Enter password:
MariaDB [(none)]> GRANT REPLICATION SLAVE ON *.* TO 'repl'@'192.168.%' IDENTIFIED BY 'replpass';
Query OK, 0 rows affected (0.00 sec)

MariaDB [(none)]> GRANT SUPER,REPLICATION SLAVE,REPLICATION CLIENT,PROCESS ON *.* TO 'repl'@'localhost' IDENTIFIED BY 'replpass';
Query OK, 0 rows affected (0.00 sec)

MariaDB [(none)]> FLUSH PRIVILEGES;
Query OK, 0 rows affected (0.00 sec)

MariaDB [(none)]> SELECT host, user FROM mysql.user;
+-----------+------+
| host      | user |
+-----------+------+
| 127.0.0.1 | root |
| 192.168.% | repl |
| ::1       | root |
| localhost | repl |
| localhost | root |
+-----------+------+
5 rows in set (0.00 sec)

MariaDB [(none)]> exit;

</pre>


### 2台で協調しながら進めます

- /etc/hostsに他方のクラスタのホストを作成
  - 1号機

<pre>

# vi /etc/hosts

127.0.0.1   localhost localhost.localdomain localhost4 localhost4.localdomain4 kvm-c7-db1
::1         localhost localhost.localdomain localhost6 localhost6.localdomain6
192.168.22.116 kvm-c7-db2

</pre>

  - 2号機

<pre>
# vi /etc/hosts

127.0.0.1   localhost localhost.localdomain localhost4 localhost4.localdomain4 kvm-c7-db2
::1         localhost localhost.localdomain localhost6 localhost6.localdomain6
192.168.22.115 kvm-c7-db1

</pre>


- corosyncで互いのサーバで疎通が取れて、認証が通るか確認します。
  - 1号機

<pre>

#  pcs cluster auth "kvm-c7-db1" "kvm-c7-db2" -u hacluster -p hapass
kvm-c7-db1: Authorized
kvm-c7-db2: Authorized

</pre>

  - 2号機

<pre>

# pcs cluster auth "kvm-c7-db1" "kvm-c7-db2" -u hacluster -p hapass
kvm-c7-db1: Already authorized
kvm-c7-db2: Already authorized

</pre>

- 2台セット動作するように設定します。
  - 1号機で実施します。2号機は不要です。

<pre>

# pcs cluster setup --name cluster_mariadb "kvm-c7-db1" "kvm-c7-db2"
Shutting down pacemaker/corosync services...
Redirecting to /bin/systemctl stop  pacemaker.service
Redirecting to /bin/systemctl stop  corosync.service
Killing any remaining services...
Removing all cluster configuration files...
kvm-c7-db1: Succeeded
kvm-c7-db2: Succeeded
Synchronizing pcsd certificates on nodes kvm-c7-db1, kvm-c7-db2...
kvm-c7-db1: Success
kvm-c7-db2: Success

Restaring pcsd on the nodes in order to reload the certificates...
kvm-c7-db1: Success
kvm-c7-db2: Success

</pre>

- stonith仮想マシンなのでstonithは無効にしておきます。
  - 1号機のみで実施。

<pre>

# pcs property set stonith-enabled=false

</pre>

- クラスタとして起動させます。
  - 1号機で実施します。2号機は不要です。

<pre>

# pcs cluster start --all
# pcs status
Cluster name: cluster_mariadb
Last updated: Sun Nov 27 21:43:23 2016          Last change: Sun Nov 27 21:42:10 2016 by root via cibadmin on kvm-c7-db1
Stack: corosync
Current DC: kvm-c7-db1 (version 1.1.13-10.el7_2.4-44eb2dd) - partition WITHOUT quorum
2 nodes and 0 resources configured

Online: [ kvm-c7-db1 ]
OFFLINE: [ kvm-c7-db2 ]

Full list of resources:


PCSD Status:
  kvm-c7-db1: Online
  kvm-c7-db2: Online

Daemon Status:
  corosync: active/disabled
  pacemaker: active/disabled
  pcsd: active/enabled

</pre>

- 2号機(=slaveになるサーバ)をstandbyにします
  - 1号機で実施します
  - pcs statusを実行すると、2号機がOFFLINEになります
  - これはクラスタを開始すると2号機が自動でレプリケーションが開始されてしまうことを防止するためです。

<pre>

# pcs cluster standby kvm-c7-db2

</pre>

- 管理するリソースを追加します。
  - 1号機のみで実施します。

  - xmlファイルを生成します。（最後にcib-pushするまでpacemakerに反映されません）

<pre>

# pcs cluster cib dbms_mariadb_cluster.xml
# cat dbms_mariadb_cluster.xml
→今いるディレクトリの下にできます。

</pre>

  - 仮想IPを追加します

<pre>

# pcs -f dbms_mariadb_cluster.xml resource create resource_dbms_vip ocf:heartbeat:IPaddr2 ip=192.168.22.117 cidr_netmask=24 op monitor interval=10s

</pre>

  - mariadb

<pre>

# pcs -f dbms_mariadb_cluster.xml resource create resource_dbms_mariadb ocf:heartbeat:mysql
# pcs -f dbms_mariadb_cluster.xml resource update resource_dbms_mariadb binary=/usr/bin/mysqld_safe datadir=/var/lib/mysql log=/var/log/mariadb/mariadb.log pid=/run/mariadb/mariadb.pid
# pcs -f dbms_mariadb_cluster.xml resource update resource_dbms_mariadb replication_user=repl replication_passwd=replepass
# pcs -f dbms_mariadb_cluster.xml resource op add resource_dbms_mariadb notify interval=0 timeout=90s

</pre>

  - 仮想IPとmariadbプロセスをグルーピングにします。

<pre>

# pcs -f dbms_mariadb_cluster.xml resource group add clustergroup_dbms_mariadb resource_dbms_vip resource_dbms_mariadb

</pre>

- 設定したXMLの内容をpacemakerへ反映します。

<pre>

# pcs cluster cib-push dbms_mariadb_cluster.xml
CIB updated

</pre>

- 反映されたクラスタの状態を確認します。

<pre>

# pcs resource
 Resource Group: clustergroup_dbms_mariadb
      resource_dbms_vip  (ocf::heartbeat:IPaddr2):       Stopped
      resource_dbms_mariadb      (ocf::heartbeat:mysql): Stopped

# pcs status
Cluster name: cluster_mariadb
Last updated: Sun Nov 27 23:20:32 2016          Last change: Sun Nov 27 23:20:05 2016 by root via cibadmin on kvm-c7-db1
Stack: corosync
Current DC: kvm-c7-db1 (version 1.1.13-10.el7_2.4-44eb2dd) - partition WITHOUT quorum
2 nodes and 2 resources configured

Online: [ kvm-c7-db1 ]
OFFLINE: [ kvm-c7-db2 ]

Full list of resources:

 Resource Group: clustergroup_dbms_mariadb
     resource_dbms_vip  (ocf::heartbeat:IPaddr2):       Stopped
     resource_dbms_mariadb      (ocf::heartbeat:mysql): Stopped

PCSD Status:
  kvm-c7-db1: Online
  kvm-c7-db2: Online

Daemon Status:
  corosync: active/disabled
  pacemaker: active/disabled
  pcsd: active/enabled

</pre>

- mariadbでマスターとした1号機でmasterと指定する宣言をします
  - clone-max=2なのは、2台でクラスタを組んでいるためです。
  - 状態を確認します

<pre>

# pcs resource master mariadb_replica resource_dbms_mariadb master-max=1 master-node-max=1 clone-max=2 clone-node-max=1 notify=true

[root@kvm-c7-db1 ~]# pcs resource
 Resource Group: clustergroup_dbms_mariadb
     resource_dbms_vip  (ocf::heartbeat:IPaddr2):       Stopped
 Master/Slave Set: mariadb_replica [resource_dbms_mariadb]
     Stopped: [ kvm-c7-db1 kvm-c7-db2 ]

# pcs status
Cluster name: cluster_mariadb
Last updated: Sun Nov 27 23:32:57 2016          Last change: Sun Nov 27 23:32:44 2016 by root via cibadmin on kvm-c7-db1
Stack: corosync
Current DC: kvm-c7-db1 (version 1.1.13-10.el7_2.4-44eb2dd) - partition WITHOUT quorum
2 nodes and 3 resources configured

Online: [ kvm-c7-db1 ]
OFFLINE: [ kvm-c7-db2 ]

Full list of resources:

 Resource Group: clustergroup_dbms_mariadb
     resource_dbms_vip  (ocf::heartbeat:IPaddr2):       Stopped
 Master/Slave Set: mariadb_replica [resource_dbms_mariadb]
     Stopped: [ kvm-c7-db1 kvm-c7-db2 ]

PCSD Status:
  kvm-c7-db1: Online
  kvm-c7-db2: Online

Daemon Status:
  corosync: active/disabled
  pacemaker: active/disabled
  pcsd: active/enabled

</pre>

## mariadbでレプリケーションするデータベースを生成します。
- 1号機で実行します

<pre>

# systemctl start mariadb
# mysql -u root -p
Enter password:
MariaDB [(none)]> CREATE DATABASE appdb CHARACTER SET utf8 COLLATE utf8_bin;

※ここからは必要に応じてスキーマを作ってください。
MariaDB [(none)]> use appdb;
MariaDB [(none)]> CREATE TABLE t_things (id int not null auto_increment primary key, name varchar(255));
MariaDB [(none)]> exit;

</pre>

## mariadbでレプリケーションの準備をします

- マスタとなる1号機で、mysqldumpを実行し、2号機へ転送します

<pre>

# mysqldump -h 127.0.0.1 -u root -p --single-transaction --master-data=2 appdb | gzip - > mysqldump.gz
# scp mysqldump.gz kvm-c7-db2:

</pre>

- 2号機でmysqldumpを取り込みデータを合わせます

<pre>

# systemctl start mariadb
# mysql -u root -p
Enter password:
MariaDB [(none)]> DROP DATABASE appdb;
MariaDB [(none)]> CREATE DATABASE appdb CHARACTER SET utf8 COLLATE utf8_bin;
MariaDB [(none)]> exit

# zcat mysqldump.gz | mysql -h 127.0.0.1 -u root -p appdb

</pre>


## クラスタを開始します。
- 1号機で実行します

<pre>


</pre>

---

## クラスタ構成を開始します


## フェールオーバーしてみます


## レプリケーションを再構成します


## 切り戻してみます






