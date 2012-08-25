## ![icon](readme_pics/pic-microchip-logo.png) PIC microcontroller用 各種ソフトウエア<!-- omit in toc -->

[Home](https://oasis3855.github.io/webpage/) > [Software](https://oasis3855.github.io/webpage/software/index.html) > [Software Download](https://oasis3855.github.io/webpage/software/software-download.html) > ***pic-program*** (this page)

<br />
<br />

- [シリアル出力 交流電流計 (PIC 12F1822)](#シリアル出力-交流電流計-pic-12f1822)
- [温湿度センサー DHT11を使う (PIC 12F1822)](#温湿度センサー-dht11を使う-pic-12f1822)
- [粉塵センサー GP2Y1010AU0Fを使う (PIC 12F1822)](#粉塵センサー-gp2y1010au0fを使う-pic-12f1822)
- [PIC Meter : シリアルデータ受信・グラフ化ソフトウエア for Windows](#pic-meter--シリアルデータ受信グラフ化ソフトウエア-for-windows)
- [シリアル接続温度計 (PIC 12F675)](#シリアル接続温度計-pic-12f675)
- [ログデータ保存機能つきLCD表示温度計 (PIC 12F1822)](#ログデータ保存機能つきlcd表示温度計-pic-12f1822)
- [最低・最高温度記憶機能つきLCD表示温度計 (PIC 16F690)](#最低最高温度記憶機能つきlcd表示温度計-pic-16f690)

<br />
<br />

## シリアル出力 交流電流計 (PIC 12F1822) 

AC電流測定用の変流器（CT）を用いてAC電流を測るPICマイコンを利用した測定器。2秒に1回の電流値がシリアル出力されるので、それをPC等で取り込むことで電流ロガーとして利用できる

[ソフトウエアのダウンロードおよびドキュメントは ct-ammeter-12f1822](ct-ammeter-12f1822/README.md) (2016/07/17)

<br />
<br />

## 温湿度センサー DHT11を使う (PIC 12F1822)

1個100円程度のデジタル温湿度センサーDHT11。整数値しか取り出せず誤差も±2℃・±5％RHと大きいが、低価格が魅力のセンサー。これをPICマイコンで使うためのサンプル回路とソフトウエア

[ソフトウエアのダウンロードおよびドキュメントは dht11-humid-12f1822](dht11-humid-12f1822/README.md) (2015/12/31)

<br />
<br />

## 粉塵センサー GP2Y1010AU0Fを使う (PIC 12F1822)

eBayで1個500円程度の粉塵センサー。対象とする粉塵はSPMなのかPM2.5なのか…。癖があるインターフェースだが、これをPICマイコンから使うためのサンプル回路とソフトウエア

[ソフトウエアのダウンロードおよびドキュメントは dustsensor-12f1822](dustsensor-12f1822/README.md) (2016/01/17)

<br />
<br />

## PIC Meter : シリアルデータ受信・グラフ化ソフトウエア for Windows

シリアル接続でデータを受信しグラフを表示する。また同時にデータをファイルに記録することも可能

[ソフトウエアのダウンロードおよびドキュメントは picmeter-win](picmeter-win/README.md) (2013/02/09)

<br />
<br />

## シリアル接続温度計 (PIC 12F675)

PIC 12F675にLM35DZを接続して、シリアル経由でデータを出力する最も単純な温度計ユニット

[ソフトウエアのダウンロードおよびドキュメントは thermometer-12f675](thermometer-12f675/README.md) (2013/02/09)


<br />
<br />

## ログデータ保存機能つきLCD表示温度計 (PIC 12F1822)

PIC 12F1822とLM35、I2C接続のEEPROMを用いてログデータ保存機能の付いた温度計ユニットの製作。測定値をリアルタイムでシリアル出力するとともに、端末側からコマンド送信することで、保存したログデータの読み出し・消去などが可能

[ソフトウエアのダウンロードおよびドキュメントは thermometer-12f1822](thermometer-12f1822/README.md) (2013/02/09)

<br />
<br />

## 最低・最高温度記憶機能つきLCD表示温度計 (PIC 16F690)

PIC 16F690とLM35を用いて、最低・最高温度記憶機能の付いた温度計ユニットの製作。測定値をリアルタイムでシリアル出力できる

[ソフトウエアのダウンロードおよびドキュメントは thermometer-16f690](thermometer-16f690/README.md) (2012/08/26)

