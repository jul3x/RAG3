#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd $DIR

ERROR_1=0
ERROR_2=0

function check()
{
    ERROR=0
    for f in $(find tests/$1 -type f -name "*.j3x")
    do
        echo "$f:"
        ./Testj3x $f >> /dev/null
        if [[ $? -ne $2 ]] ; then
            echo -e "\e[92mOK!\e[0m"
	else
            ERROR=1
            echo -e "\033[31mSEMANTIC CHECKER ERROR!\e[0m"
        fi
    done

    return $ERROR
}

function print()
{
    if [[ $1 -eq 0 ]] ; then
        echo -e "\e[92mOK!\e[0m"
    else
        echo -e "\033[31mFAILED!\e[0m"
    fi
}

if [[ $# -eq 0 ]] ; then
    echo "BAD TESTS CHECK:"
    check "bad" 0
    ERROR_1=$?

    echo -e "\n\nGOOD TESTS CHECK:"
    check "good" 1
    ERROR_2=$?

    echo -e "\nBad: "
    print $ERROR_1

    echo "Good: "
    print $ERROR_2
else
    echo $1" check:"
    check $1 $2
    ERROR=$?
    echo -e "\nResult:"
    print $ERROR
fi
