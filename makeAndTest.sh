dir64="build_64_$RANDOM";
mkdir $dir64 > /dev/null;
pushd $dir64 > /dev/null;
echo "64 bit in " $dir64;
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=x86_64 ../code/  > /dev/null && make -j8 > /dev/null && ./testKotton;
popd  > /dev/null;
rm -rf $dir64 > /dev/null;

dir32="build_32_$RANDOM";
mkdir $dir32 > /dev/null
pushd $dir32 > /dev/null;
echo "32 bit in" $dir32;
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=i386 ../code/ > /dev/null && make -j8 > /dev/null && ./testKotton;
popd > /dev/null;
rm -rf $dir32 > /dev/null;


