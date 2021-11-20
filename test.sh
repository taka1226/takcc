#!/bin/bash

assert(){
    expected="$1"
    input="$2"

    ./takcc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 4 'aaa=4;'
assert 16 'aaa=4;bb=4;cc=aaa * bb;'

echo OK
