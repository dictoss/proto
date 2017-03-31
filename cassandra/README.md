
# cassndra-3.0の調査

## 情報 (結構古い情報が出てくる)

- Cassandraメモその1(インストール、データモデル、データ操作について)
  - https://hiroakis.com/blog/2012/10/17/cassandra%E3%83%A1%E3%83%A2%E3%81%9D%E3%81%AE1%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB%E3%80%81%E3%83%87%E3%83%BC%E3%82%BF%E3%83%A2%E3%83%87%E3%83%AB%E3%80%81%E3%83%87%E3%83%BC%E3%82%BF/
- 「Cassandraのデータ設計で注意していること」
  - http://ameblo.jp/principia-ca/entry-11886808914.html

## データ格納の器

### keyspace

- RDBMSのdatabaseのような位置づけ
- cqlで"describe keyspaces;"を実行すると、スキーマを参照できる


### columnfamily(table)

- RDBMSのtableのような位置づけ
- columnfamilyがcassandra用語としては正しい
  - "CREATE TABLE"は"CREATE COLUMNFAMILY"のエイリアス
- cqlで"describe tables;"と実行すると、スキーマを参照できる
- default_time_to_liveというパラメータでTTLが指定可能


### row

- RDBMSのレコードのような位置づけ
- rowには複数のカラムが含まれる。
- columnfamilyの全ROWの中で、1つまたは複数のカラムを用いて一意なROWを特定できるようにPrimary keyを付与する必要がある
- keyがいくつかあり、その意味
  - http://stackoverflow.com/questions/24949676/difference-between-partition-key-composite-key-and-clustering-key-in-cassandra
  - primary key
    - columnfamilyの中で一意なROWを特定する1つまたは複数のカラムで構成したキー。要はユニークキー。KVSでいうところのkeyである。
    - CREATE columnfamily文の中で、"PRIMARY KEY"と指定する。
    - 複数のカラムを使う場合、"PRIMARY KEY ( (a1, a2), b1, b2) )"のように書ける。
    - "PRIMARY KEY"に指定したカラムはSELECT、UPDATE、DELETEのWHERE句に指定して処理する。
  - composite key(複合キー)
    - ドキュメントでは、"Using a composite partition key"と用いられている。
      - https://docs.datastax.com/en/cql/3.1/cql/cql_reference/refCompositePk.html
    - ドキュメントでは、"Using a compound primary key"と用いられている。
      - https://docs.datastax.com/en/cql/3.1/cql/cql_using/use_compound_keys_t.html
    - "composite partition key"、"compound primary key"の表現がごっちゃになった模様。定義が厳密でない用語と思われ、どちらかを差すものと思われる。
      - 要は「複数のカラムで構成したパーティションキー」または「複数のカラムで構成したプライマリーキー」のどちらかを言いたいあいまいな表現と考えられる。
  - partition key
    - primary keyの中で、最初に位置するカラムのことを差す
    - primary keyのカラムが1つの場合は、primary key = partition keyとなる
    - partition keyは、複数のcassandraノードに分割して保存する条件として利用される。なるべくユニーク性が高い値の方がよい（1つのcassandraノードへアクセスが集中しにくくなる）
    - partition keyは1つまたは複数のカラムで構成できる
      - "PRIMARY KEY ( (a1, a2), b1, b2) )"のように書くとa1,a2カラムの組み合わせてpartition keyとして扱われる
      - これは複数のcassandraノードに分散配置しやすいようにするため
  - clustering columns(clustering key)
   - "on the clustering columns"という表現があることから、"clustering key"という言葉は存在しないものと思われる。正しくはclustering columnsと思われる。
     - https://docs.datastax.com/en/cql/3.1/cql/ddl/ddl_compound_keys_c.html
   - primary keyの中で、最初のキーをpartition keyといい、2番目以降のキーをclustering columnsという。
   - cassandraへ保存されるときにclustering columnsの順にソートされて保存される。
     - partition keyが同じデータ群はclustering columnsでソートした状態で、1つのcassandraノードに格納される。


### columns

- RDBMSのカラム（フィールド）のような位置づけ
- 自動で最終更新タイムスタンプが付与される
- カラムは、カラム名(key)、値(value)、タイムスタンプ、TTLが設定できる
  - ROWでなくて、カラムにTTLが設定できます
  - 疑問）すべてのカラムがTTLで消滅した場合、そのROWも消滅するの？
- ROWごとにcolumnsの数は違って問題ない
  - カラム名に連番を含める方法がとれるかも。(count: 3, col1:"a", col2:"b", col3:"c"のような感じ)
  - カラムファミリでカラム数を統一し、あるカラムにJSON文字列を入れて、アプリ処理する方法もあるかもしれない。
  - カラム数が無限に増える実装は避けたほうがいいらしい。
- カラムにindexを付与しないほうがいいパターン
  - https://docs.datastax.com/en/cql/3.1/cql/ddl/ddl_when_use_index_c.html
    - そのカラムの値がとてもある値に偏る場合（例：性別）
    - カウンタのように使っているカラム
    - 頻繁に更新、削除されるカラム
    - To look for a row in a large partition unless narrowly queried
- indexを付けたカラムのみを検索対象とすることは可能ではある。その代わり、ALLOW FILTERING句を付与する必要がある。
  - RDBMSのbtree検索をやっていることを同じ。
  - nosqlを使っているのに、（パーティションキーを指定しない状態で）btree検索するのはなるべく避けた方がいいのかもしれない。


### super columns

- カラムの集合であり、カラムファミリーの部分集合
- なくても問題ない。むしろ非推奨の模様
  - https://www.ibm.com/developerworks/jp/opensource/library/os-apache-cassandra/
    - 「スーパー・カラムと OrderPreservingPartitioner は推奨されません」
    - スーパーカラムを使わなくてもカラムで代替可能。
    - スーパーカラムはセカンダリインデックスの対象にできないため使い勝手が悪い
    - そのため、スーパーカラムは使わないようにするのがよいです。
- CQL 2.0でdeprecated となり、CQL 3.0でサポートがなくなった


## データ構造との戦略

- パーティションキーの選定
  - 考察
    - 顧客IDやユーザIDなどの特定のユーザなど、一度のアクセスで大量のデータをread、writeする単位で単一のノードに乗せるようにするべきか、複数のノードに分散して配置すべきか
      - http://ameblo.jp/principia-ca/entry-11886808914.html
      - 特定のnodeのみ負荷があがる場合があるため、別のノードへのHOPする処理時間はかかるが、１つの顧客やユーザのデータを1つのノードのみで完結するようなパーティションキーは避けた方がいい
      - 日付を付けた

- プライマリキーの定義
  - これはアプリケーションによる。


## cassandraの使い道

- 一時的なキャッシュのような使い方
  - tableにTTLを指定して使う。
  - ほぼmemcachedと同じ使い方。パーティションキーはある程度適当でもよさそう。

- 一定期間後の過去データは捨ててもいい系のデータを保存する

- 永続的に記録するデータを格納する場合



## サーバ運用

### partitioner

- partition keyをどのように判定して複数のcassandraノードに配置されするか
  - https://docs.datastax.com/en/cassandra/3.0/cassandra/configuration/configCassandra_yaml.html?hl=orderpreservingpartitioner
  - Murmur3Partitionerが推奨でデフォルト
  - RandomPartitionerも選択可能
  - ByteOrderedPartitioner、OrderPreservingPartitionerはdeprecated

### 最小何台から始めるか


### ノードを追加するときにどうするか


### ノードを減らすときにどうするか


### ノードが障害になったときに、別のノードへコピーさせるときはどうするか


### メモリ不足の兆候はつかめるか
