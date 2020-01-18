
# PINE A64+

## ボード情報

- webサイト
  - https://www.pine64.org/devices/single-board-computers/pine-a64/
  - 発売日は、2017/05/02
  - 64bit ARM CPUであるAllwinner's A64 Quad-Core ARM Cortex A53 64-Bit SOCを搭載
  - メモリが512MB版、1GB版、2GB版の3種類ある
    - PINE A64が512MB版
    - PINE A64+が1GB版と2GB版

- ボードの解説記事
  - https://linux-sunxi.org/Pine64

## 特徴

- arm64版debianが素で動く
  - PINE A64+はCPUもソフトウェアも64bit
  - Raspberry Pi 3以降はCPUは64bitだが、OSであるRaspbianがarmhであり32bit
- Debian 10 busterからDebian Installerが用意されているボード
  - [Debian Installer Buster版](http://ftp.debian.org/debian/dists/buster/main/installer-arm64/current/images/u-boot/pine64_plus.img.gz)
  - [Debian Installer unstable版](https://d-i.debian.org/daily-images/arm64/daily/u-boot/pine64_plus.img.gz)
  - Installするためのドキュメント
    - [InstallingDebianOnPINE64PINEA64](https://wiki.debian.org/InstallingDebianOn/PINE64/PINEA64)

## PINアサイン

### PINアサインのドキュメント

- http://files.pine64.org/doc/Pine%20A64%20Schematic/Pine%20A64%20Pin%20Assignment%20160119.pdf
  - PIN群は3つある
    - Pi-2 Connector （USBとHDMI端子の間にある40pin）
    - Exp Connector （microsdカードスロットにすぐ横にある10pin）
    - Euler "e" Connector （Expの隣にある 40pin）

### USBでシリアルコンソールをとる方法

- Exp Connector (10pinのところ)からシリアルコンソールを取れる
  - 7 (TXD), 8 (RXD), 9 (GND)につなぐ
    - [このUSBシリアル変換ケーブル](https://www.switch-science.com/catalog/1196/)の場合、7 (白)、8 (緑)、9 (黒)でつなげばよい（赤は差さない）
- Debian Installerでインストールした場合はOS側はすでに設定済
  - `# cat /proc/cmdline`
    - console=ttyS0,115200 quiet
  - `# systemctl status serial-getty@ttyS0.service`
    - 起動中
