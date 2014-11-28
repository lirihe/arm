#!/bin/sh

echo "" > temp.md
echo "\chapter{NanoMind}" >> temp.md
cat README.md >> temp.md
cat INSTALL.md >> temp.md
cat doc/*.md >> temp.md
echo "\chapter{CSP}" >> temp.md
cat lib/libcsp/doc/*.md >> temp.md
echo "\chapter{System library}" >> temp.md
cat lib/libgomspace/doc/*.md >> temp.md
echo "\chapter{CDH}" >> temp.md
cat lib/libcdh/doc/*.md >> temp.md
echo "\chapter{IO}" >> temp.md
cat lib/libio/doc/*.md >> temp.md
echo "\chapter{Storage}" >> temp.md
cat lib/libstorage/doc/*.md >> temp.md
pandoc --template=gomspace.latex --toc -o gs-sw-doc.pdf -N \
	--variable=ref:"GS-SW-DOC" \
	--variable=gitref:`git describe` \
	--variable=title:"GOMX-1" \
	--variable=frontimage:/home/johan/.pandoc/gomspacelogo.png \
	--variable=subtitle:"Software Documentation Package" \
	temp.md
rm temp.md
