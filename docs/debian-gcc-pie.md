# Debian 9 から有効でビルドされているGCCのPIEについて

## 調べようと思ったきっかけ

- Debian 9 のリリースノートに開発向け情報として記述がある

  - 「5.1.5. 実行ファイルはデフォルトで PIE (position independent executables) が有効でコンパイルされています」

    - https://www.debian.org/releases/stable/amd64/release-notes/ch-information.ja.html#pie-is-now-default

- UNIXで共有ライブラリをコンパイルする場合、PIC (Position Independent Code)を有効にするのが普通。オプションは`-fpic`、`-fPIC`の2種類がある。

- `-pie`オプションがあるのは知らなかった。

- Debian上でパッケージを作るときにどう影響してくるのかわからないため、調べてみよう

## 参考情報

- ASLRとKASLRの概要

  - https://qiita.com/satoru_takeuchi/items/5c80c4e255e21c5b4b8a


## Debianにおけるgccの採用バージョン

- Debian 7 wheezy

  - gcc-4.4 パッケージ (gcc-4.4.7)
  - gcc-4.6 パッケージ (gcc-4.6.3)
  - gcc-4.7 パッケージ (gcc-4.7.)2

- Debian 8 jessie

  - gcc-4.8 パッケージ (gcc-4.8.4)
  - gcc-4.9 パッケージ (gcc-4.9.2)

- Debian 9 stretch

  - gcc-6 パッケージ (gcc-6.3.0)
    - "--enable-default-pie" が有効でgccをビルドしている

- Debian unstable (buster) (2017-10-21現在)

  - gcc-6 パッケージ (gcc-6.4.0)
    - "--enable-default-pie" が有効でgccをビルドしている
  - gcc-7 パッケージ (gcc-7.2.0)
    - gcc パッケージをインストールすると、gcc-7がインストールされます
    - "--enable-default-pie" が有効でgccをビルドしている

## GCCのPIE (position independent executable) について

### PIEとは

- PIE (position independent executable)という機能で、日本語では「位置独立実行形式」という。

- 共有ライブラリはPICとしてコンパイルするが、PIEは実行ファイル版のようなもの

- gcc-7のオプションでは、`-fPIE`、`-pie`、`-no-pie`がある。


## ディストリビューションの状況

- Debian

  - https://wiki.debian.org/Hardening/PIEByDefaultTransition
  - https://lintian.debian.org/tags/hardening-no-pie.html
  - gcc-7のソースパッケージの"debian/rules.defs"に書いてある

    - with_pie で検索すると出てくる
    - "--enable-default-pie"というビルドオプションを指定してgccをビルドしている
      - gcc-6.0-7から有効になっている。
    - linuxの各arch、kfreebsd-*も対象。hurdは入っていない。

- Fedora

  - https://fedoraproject.org/wiki/Changes/Harden_All_Packages

- FreeBSD

  - clangコマンドにも `-fPIE`、`-pie` オプションがある
