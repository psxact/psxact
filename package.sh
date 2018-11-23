#!/bin/bash

set -e

cp bin/psxact .

commit_date=`git show -s --format=%cI $CI_COMMIT_SHA`

version=`date --date="$commit_date" +"%Y.%m.%d"`
archive=psxact_$version-$CI_COMMIT_SHA.tar.bz2

tar -cvjSf $archive psxact

rm psxact
