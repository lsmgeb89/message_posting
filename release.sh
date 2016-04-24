#!/bin/bash
#

readonly NAME="cs5348_project_3"
readonly RELEASE_FOLDER="${HOME}/${NAME}"
readonly RELEASE_ZIP="${HOME}/${NAME}.zip"

# delete previous release zip
if [ -f "$RELEASE_ZIP" ]; then
  rm "$RELEASE_ZIP"
fi

mkdir -p "$RELEASE_FOLDER"/src
# copy source files
cp -ar message_posting/client "$RELEASE_FOLDER"/src
cp -ar message_posting/server "$RELEASE_FOLDER"/src
cp -ar message_posting/utils "$RELEASE_FOLDER"/src
# copy readme.txt
cp message_posting/readme.txt "$RELEASE_FOLDER"
# copy CMakeLists
cp message_posting/CMakeLists.txt "$RELEASE_FOLDER"/src
# compile summary.tex
pushd summary > /dev/null 2>&1
pdflatex -output-directory="$RELEASE_FOLDER" summary.tex > /dev/null 2>&1
popd > /dev/null 2>&1
# compile design.tex
pushd design > /dev/null 2>&1
pdflatex -output-directory="$RELEASE_FOLDER" design.tex > /dev/null 2>&1
popd > /dev/null 2>&1
# clean auxiliary files
pushd "$RELEASE_FOLDER" > /dev/null 2>&1
rm *.aux *.log
popd > /dev/null 2>&1
# package all files
pushd "${HOME}" > /dev/null 2>&1
zip -r "$RELEASE_ZIP" "$NAME"/*
chmod 777 "$RELEASE_ZIP"
popd > /dev/null 2>&1

# delete release folder
if [ -d "$RELEASE_FOLDER" ]; then
  rm -rf "$RELEASE_FOLDER"
fi
