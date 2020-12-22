#!/bin/sh

# Create test directory with some phlog posts
mkdir testdir
touch testdir/2020-01-03__Test_1.md
touch testdir/2020-01-01__Test_2.txt
touch testdir/2020-01-02__Test_3.txt
touch testdir/2019-11-25__Test_4.md
# Include a sub-directory
mkdir testdir/2019-12-05__Test_dir_1
# Only md, txt and glink files are handled
touch testdir/2019-11-27__Test_5.png
touch testdir/2019-11-26__Test_6
touch testdir/Test7.md
touch testdir/xx__Test8.txt
# Create some archive directories
mkdir testdir/AR_2018
mkdir testdir/AR_2019
# Create a glink file
echo "selector/file.md	serverx.com	70" >> testdir/2020-08-15__Test_GLink.glink

# Create a gophermap template
# Line 1 should be included in the generated gophermap
echo "line 1" >> testdir/map.template
# Invalid placeholder, line will be passed unchanged to the generated gophermap
echo "line 2 {{ TEST }}" >> testdir/map.template
# Include the regular phlog list
echo "line 3 {{ FILE_LIST }} + text" >> testdir/map.template
# UTF-8
echo "今日は！" >> testdir/map.template
# Include the archive directory list
echo "" >> testdir/map.template
echo "Archives:" >> testdir/map.template
echo "Some ignored text {{ ARCHIVE_LIST }} ignored again" >> testdir/map.template
# Include the current date and time
echo "Ĝisdatigita je {{ DATE_TIME }}." >> testdir/map.template

# Generate and print the gophermap
./navopher testdir map.template
cat testdir/gophermap
