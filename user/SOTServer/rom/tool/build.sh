#!/bin/sh

# basic setting
_ROM_ORG=0x00100000
_FAT_SIZE=0x00010000
_MAX_FILE_NUM=$[$_FAT_SIZE / 0x80]
_ROOT_DIR=../root
_GZIP_DIR=../rom/ziped
_FILE_LIST_FILE=../tool/file_list
_OUTPUT_ROM=../rom/sot_rom.bin

# basic functions
function dump_file_list()
{
    for i in `ls $1`
    do
        if [ -d $1/$i ]
        then
            dump_file_list $1/$i
        else
            path=$1/$i
            path=${path#*.}
            if [ ${#path} -gt 64 ]
            then
                echo 'filename is greater than SOT_ROM limited.'
                exit 1
	    else
                #gzip -c $_ROOT_DIR$path > $_GZIP_DIR$path
                echo $path >> $_FILE_LIST_FILE
            fi
        fi
    done
}

function get_mime()
{
    filename=$1
    ext=${filename##*.}
    case $ext in
# you can add your mime type here, but do not exceed the 56 chars limit
        "txt") echo -n text/plain;;
        "htm") echo -n text/html;;
        "html") echo -n text/html;;
        "css") echo -n text/css;;
        "js") echo -n application/x-javascript;;
        "png") echo -n image/png;;
        "jpg") echo -n image/jpeg;;
        "json") echo -n application/json;;
        *) echo -n application/octet-stream;;
    esac
}

# prepare file to pack
echo "prepare files ... "

rm -rf $_GZIP_DIR
cp -R $_ROOT_DIR $_GZIP_DIR

# get file list
echo "scan files ... "

cd $_ROOT_DIR
: > $_FILE_LIST_FILE
dump_file_list .
_FILE_NUM=`sed -n $= $_FILE_LIST_FILE`

# check if file number is valid
if [ $_FILE_NUM -gt $_MAX_FILE_NUM ]
then
    echo 'file number is greater than _FAT_SIZE limited.'
    exit 1
fi

: > $_OUTPUT_ROM
# set FAT table
echo "write file allocate table ... "

_CURRENT_LOCATION=0
_CURRENT_FILE_MIME=0
_CURRENT_FILE_NAME=0
_CURRENT_HEAD_SIZE=0
_CURRENT_FILE_SIZE=0
_CURRENT_FILE_NUM=0
_CURRENT_ROM_SIZE=0

while read filename
do

# finish 64 bytes filename setion
    _CURRENT_FILE_NAME="$filename"
    echo -n $_CURRENT_FILE_NAME >> $_OUTPUT_ROM
    _CURRENT_HEAD_SIZE=${#_CURRENT_FILE_NAME}
    while(( $_CURRENT_HEAD_SIZE < 64 ))
    do
        echo -e -n "\x00" >> $_OUTPUT_ROM
        _CURRENT_HEAD_SIZE=$[$_CURRENT_HEAD_SIZE + 1]
    done

# finish 56 bytes mime section
    _CURRENT_FILE_MIME=$(get_mime $_CURRENT_FILE_NAME)
    echo -n $_CURRENT_FILE_MIME >> $_OUTPUT_ROM
    _CURRENT_HEAD_SIZE=$[${#_CURRENT_FILE_MIME} + $_CURRENT_HEAD_SIZE]
    while(( $_CURRENT_HEAD_SIZE < 120 ))
    do
        echo -e -n "\x00" >> $_OUTPUT_ROM
        _CURRENT_HEAD_SIZE=$[$_CURRENT_HEAD_SIZE + 1]
    done

# finish 4 bytes file length section
    _CURRENT_FILE_SIZE=$(stat -c %s $_GZIP_DIR$_CURRENT_FILE_NAME)
    echo -e -n `printf %s%02x%s%02x%s%02x%s%02x "\\x" $[$_CURRENT_FILE_SIZE>>0&0xFF] "\\x" $[$_CURRENT_FILE_SIZE>>8&0xFF] "\\x" $[$_CURRENT_FILE_SIZE>>16&0xFF] "\\x" $[$_CURRENT_FILE_SIZE>>24&0xFF]` >> $_OUTPUT_ROM
    _CURRENT_HEAD_SIZE=$[$_CURRENT_HEAD_SIZE + 4]

# make 4 bytes aligned size    
    while(( $[$_CURRENT_FILE_SIZE % 4] ))
    do
        _CURRENT_FILE_SIZE=$[$_CURRENT_FILE_SIZE + 1]
    done

# finish 4 bytes file location section
     echo -e -n `printf %s%02x%s%02x%s%02x%s%02x "\\x"  $[$_CURRENT_LOCATION>>0&0xFF] "\\x" $[$_CURRENT_LOCATION>>8&0xFF] "\\x" $[$_CURRENT_LOCATION>>26&0xFF] "\\x" $[$_CURRENT_LOCATION>>24&0xFF]` >> $_OUTPUT_ROM   
    _CURRENT_LOCATION=$[$_CURRENT_LOCATION + $_CURRENT_FILE_SIZE]
    _CURRENT_HEAD_SIZE=$[$_CURRENT_HEAD_SIZE + 4]

# refresh _CURRENT_ROM_SIZE
    _CURRENT_ROM_SIZE=$[$_CURRENT_HEAD_SIZE + $_CURRENT_ROM_SIZE]

done < $_FILE_LIST_FILE

# compelete empty data
while(( $_CURRENT_ROM_SIZE < $_FAT_SIZE ))
do
    echo -e -n "\x00" >> $_OUTPUT_ROM
    _CURRENT_ROM_SIZE=$[$_CURRENT_ROM_SIZE + 1]
done

# append file
echo "write files ... "

while read filename
do
    _CURRENT_FILE_NAME="$filename"
    _CURRENT_FILE_SIZE=$(stat -c %s $_GZIP_DIR$_CURRENT_FILE_NAME)
# write file
    cat $_GZIP_DIR$_CURRENT_FILE_NAME >> $_OUTPUT_ROM
    _CURRENT_ROM_SIZE=$[$_CURRENT_ROM_SIZE + $_CURRENT_FILE_SIZE]
# ajustment for 4 byte alignment
    while(( $[$_CURRENT_ROM_SIZE % 4] ))
    do
        echo -e -n "\x00" >> $_OUTPUT_ROM
        _CURRENT_ROM_SIZE=$[$_CURRENT_ROM_SIZE + 1]
    done
done < $_FILE_LIST_FILE

