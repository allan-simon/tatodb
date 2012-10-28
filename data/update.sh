#!/bin/sh

wget http://tatoeba.org/files/downloads/sentences.csv -O sentences.csv
wget http://tatoeba.org/files/downloads/links.csv -O links.csv

php xmlize.php > tatodb.xml
