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

assert 4 '1+6-3'
assert 42 '24+20-2'

echo OK
