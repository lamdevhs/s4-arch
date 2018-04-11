#!/bin/bash


NAME=sublime
FILE=~/$NAME.tar.bz2

wget -O $FILE https://download.sublimetext.com/sublime_text_3_build_3143_x64.tar.bz2
cd ~/
bzip2 -d $FILE; tar xvf ~/$NAME.tar
cd -
~/sublime_text_3/sublime_text .


