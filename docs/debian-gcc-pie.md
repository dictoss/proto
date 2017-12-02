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

- Address space layout randomization

  - https://en.wikipedia.org/wiki/Address_space_layout_randomization
  - The Linux PaX project first coined the term "ASLR", and published the first design and implementation of ASLR in July 2001 as a patch for the Linux kernel. It is seen as a complete implementation, providing also a patch for kernel stack randomization since October 2002.
  - The first mainstream operating system to support ASLR by default was the OpenBSD version 3.4 in 2003,[2][3] followed by Linux in 2005.
  
- ELF形式の実行ファイル

  - ELF (Executable and Linkable Format)
  - https://ja.wikipedia.org/wiki/Executable_and_Linkable_Format

- ELF実行ファイルのメモリ配置はどのように決まるのか

  - http://inaz2.hatenablog.com/entry/2014/07/27/205913

- PIE (位置独立実行形式) を作成する

  - http://0xcc.net/blog/archives/000109.html

- [PATCH] -fpie/-fPIE/-pie GCC support

  - https://gcc.gnu.org/ml/gcc-patches/2003-06/msg00140.html

- "-fPIE ("position-independent executable) option (gcc, ld)"

  - https://stackoverflow.com/questions/2463150/fpie-position-independent-executable-option-gcc-ld

  - http://www.openbsd.org/papers/nycbsdcon08-pie/


## 各ディストリビューションにおけるgccの採用バージョン

###  Debian

- 情報

  - https://wiki.debian.org/Hardening/PIEByDefaultTransition
  - https://lintian.debian.org/tags/hardening-no-pie.html
  - gcc-7のソースパッケージの"debian/rules.defs"に書いてある
  
    - with_pie で検索すると出てくる
    - "--enable-default-pie"というビルドオプションを指定してgccをビルドしている
      - gcc-6.0-7から有効になっている。
    - linuxの各arch、kfreebsd-*も対象。hurdは入っていない。

- Debian 7 wheezy

  - gcc-4.4 パッケージ (gcc-4.4.7)
  - gcc-4.6 パッケージ (gcc-4.6.3)
  - gcc-4.7 パッケージ (gcc-4.7.2)

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


### Fedora

  - https://fedoraproject.org/wiki/Changes/Harden_All_Packages

### FreeBSD

  - clangコマンドにも `-fPIE`、`-pie` オプションがある


## 実行ファイルのELF形式

- ELF (Executable and Linkable Format)とは、現在のLinuxや多くのUNIXの系譜に連なるディストリビューションが採用している実行ファイルの形式である。

- 過去には、a.out、COFFといった実行ファイルの形式が採用されていた。

- ELFにおける実行ファイルは、プログラムの実行コードやリンクするライブラリをどのメモリアドレスに配置するかは、通常リンク時に決定している。

  - そのため、実行時のメモリアドレスは毎回ほぼ同じ場所にプログラムの実行コード、ライブラリの関数等のアドレスが配置される。

  - PIEオプションは、実行ファイルがロードするメモリアドレス、ライブラリの関数等のアドレス配置位置を実行時に


## GCCのPIE (position independent executable) 

### PIEとは

- PIE (Position Independent Executable)という機能で、日本語では「位置独立実行形式」という。

- 共有ライブラリはPIC (Position Independent Code)としてコンパイルするが、PIEは実行ファイル版のようなもの

- gcc-7のオプション

  - コンパイル時オプション `-fPIE`
  - リンク時オプション `-pie`、`-no-pie`


- gcc

## 実際に試してみた

### debian stretchのデフォルト状態 (pieあり)


### debian stretchでコンパイル、リンク時に-nopieを指定


## まとめ

