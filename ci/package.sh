#!/bin/bash

set -e

cp bin/app/psxact .

COMMIT_DATE=`git show -s --format=%cI $CI_COMMIT_SHA`

VERSION=`date --date="$COMMIT_DATE" +"%Y.%m.%d"`
ARCHIVE=psxact_$VERSION-$CI_COMMIT_SHA.tar.bz2

tar -cvjSf $ARCHIVE psxact

rm psxact
