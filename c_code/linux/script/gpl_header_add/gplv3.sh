#!/bin/bash

NAME="Your Name"
EMAIL="your_email@email.com"
EXT="c cpp h java"
FILTER="Copyright"

if [ $# -eq 0 ]
then
    FILES=`find .`
else
    if test -d $1
    then
        FILES=`find $1`
    else
        FILES=$@
    fi
fi

insert_header() {
    YEAR=`date +"%Y"`
    COPYRIGHT="Copyright (C) $YEAR Ingenic Semiconductor Co., Ltd"
    sed -i -e '1i\\' $1
    sed -i -e '1i\ */' $1
    sed -i -e '1i\ *\ along with this program.  If not, see <http://www.gnu.org/licenses/>.' $1
    sed -i -e '1i\ *\ You should have received a copy of the GNU General Public License' $1
    sed -i -e '1i\ *\' $1
    sed -i -e '1i\ *\ GNU General Public License for more details.' $1
    sed -i -e '1i\ *\ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the' $1
    sed -i -e '1i\ *\ but WITHOUT ANY WARRANTY; without even the implied warranty of' $1
    sed -i -e '1i\ *\ This program is distributed in the hope that it will be useful,' $1
    sed -i -e '1i\ *\' $1
    sed -i -e '1i\ *\ (at your option) any later version.' $1
    sed -i -e '1i\ *\ the Free Software Foundation, either version 3 of the License, or' $1
    sed -i -e '1i\ *\ it under the terms of the GNU General Public License as published by' $1
    sed -i -e '1i\ *\ This program is free software: you can redistribute it and/or modify' $1
    sed -i -e '1i\ *\' $1
    sed -i -e "1i\ *\ Author: $NAME <$EMAIL>" $1
    sed -i -e "1i\ *\ $COPYRIGHT" $1
    sed -i -e '1i\ *\' $1
    sed -i -e "1i\ *\ $(basename $FILE) - here should give a brief idea of what this file does" $1
    sed -i -e "1i/*" $1
}

NUM=0
for FILE in $FILES
do
    if test -f $FILE
    then
        if [[ $(echo $EXT | grep "${FILE##*.}") != "" ]]
        then
            if ! grep "$FILTER" $FILE > /dev/null
            then
                echo "modify file : $FILE"
                NUM=$(($NUM+1))
                insert_header $FILE
            else
                echo "ignore file : $FILE"
        fi
    fi
fi
done
if [ $NUM -eq 0 ]
then
    echo "No file changed.";
else
    echo "$NUM file(s) changed.";
fi
