#!/bin/sh

mkdir testdir
touch testdir/2020-01-03__Test_1.md
touch testdir/2020-01-01__Test_2.txt
touch testdir/2020-01-02__Test_3.txt
touch testdir/2019-11-25__Test_4.md
mkdir testdir/2019-12-05__Test_dir_1
touch testdir/2019-11-27__Test_5.png
touch testdir/2019-11-26__Test_6
touch testdir/Test7.md
touch testdir/xx__Test8.txt
echo "selector/file.md	serverx.com	70" >> testdir/2020-08-15__Test_GLink.glink
echo "line 1" >> testdir/map.template
echo "line 2 {{ TEST }}" >> testdir/map.template
echo "line 3 {{ FILE_LIST }} + text" >> testdir/map.template
echo "今日は！" >> testdir/map.template
echo "Ĝisdatigita je {{ DATE_TIME }}." >> testdir/map.template
./navopher testdir map.template
cat testdir/gophermap
