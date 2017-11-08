# rocminfo
ROCm Application for Reporting System Info

## To Build
Use the standard cmake build procedure to build rocminfo. The location of ROCM
root (parent directory containing ROCM headers and libraries) must be provided
as a cmake argument. For example, building from the CMakeLists.txt directory
might look like this:

mkdir -p build

cd build

cmake -DROCM_DIR=<path to ROCM root> ..

make

cd ..


Upon a successful build the binary, rocminfo, will be in the build folder.

