set -e
set -x

rm -f hatari.zip
cd tools/hrdb
if [ -f Makefile ]; then
    make clean
fi
rm -f Makefile
rm -f .qmake.stash
rm -rf hrdb.app
qmake
make
macdeployqt hrdb.app/
zip -9 -r ../../hatari.zip hrdb.app
cd ../..

rm -rf build-hatari
mkdir build-hatari
cd build-hatari
cmake ..
make -j2
cd src
zip -9 -r ../../hatari.zip hatari.app
cd ../..
echo hatari.zip is done
