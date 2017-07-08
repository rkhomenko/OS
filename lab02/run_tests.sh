#!/usr/bin/env bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

TESTS_FOR="*************** ${YELLOW}Tests for lab02${NC} ****************\n"

BINARY=$1

TESTS=`find ./test -type d -name "test?*"`

printf "$TESTS_FOR"
for t in $TESTS; do
    printf "Running $t: ..."
    TMP_STDOUT=`mktemp`
    TMP_STDERR=`mktemp`
    $BINARY $t/1.txt $t/2.txt > $TMP_STDOUT 2> $TMP_STDERR

    if cmp -s $t/stdout.txt $TMP_STDOUT; then
        if cmp -s $t/stderr.txt $TMP_STDERR; then
            printf " ${GREEN}passed${NC}!\n"
        else
            printf " ${RED}failed${NC}!\n"
        fi
    else
        printf " ${RED}failed${NC}!\n"
    fi
    rm -f $TMP_STOUT $TMP_STDERR
done
echo "************************************************"