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
assert 12 'aaa=3;bb=4;return cc=aaa * bb;'
assert 14 'return 14;return 12 - 4;'
assert 11 'return 11;'
assert 1 'aaa = 3>=2;return aaa;'
assert 0 'return 2 == 1;'
assert 0 'return 1 != 1;'
assert 1 'return 1<2;'


echo OK
