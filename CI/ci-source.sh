# This script just creates compressed files of OpenFlipper sources

# Create Build directory and Build documentation
mkdir build-doc

pwd

ls

cd build-doc

pwd 

ls

cmake ..

pwd 

ls

make doc
cd ..

# Extract Version Information
VERSION=OpenFlipper-$(cat OpenFlipper/VERSION | grep VERSION | tr -d "VERSION=")

# Create Publishing directory
mkdir $VERSION

# Move all files into Publishing directory
mv OpenFlipper	     $VERSION/
mv Plugin-*          $VERSION/
mv PluginCollection* $VERSION/
mv Type* 	     $VERSION/
mv CMakeLists.txt    $VERSION/
mv COPYING           $VERSION/
mv Readme.md         $VERSION/

cd $VERSION
find . -name "*git*" | xargs rm -rf
cd ..

mkdir $VERSION/Documentation
mv build-doc/Build/share/OpenFlipper/Doc/DeveloperHTML/ $VERSION/Documentation/DeveloperHTML
mv build-doc/Build/share/OpenFlipper/Doc/UserHTML/ $VERSION/Documentation/UserHTML

tar cjf $VERSION.tar.bz2 $VERSION
tar czf $VERSION.tar.gz $VERSION
zip -9 -q -r $VERSION.zip $VERSION

