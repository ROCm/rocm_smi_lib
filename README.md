# rocminfo
ROCm Application for Reporting System Info

## To Build
Use the standard cmake build procedure to build rocminfo. The location of ROCM
root (parent directory containing ROCM headers and libraries) must be provided
as a cmake argument using the standard CMAKE_PREFIX_PATH cmake variable. Building
the package also requires the rocm-cmake helper files.

For example, building from the CMakeLists.txt directory
might look like this:

mkdir -p build

cd buildhttp://www.google.com/

cmake -DCMAKE_PREFIX_PATH=/opt/rocm ..

make

cd ..


Upon a successful build the binary, rocminfo, and the python script, rocm_agent_enumerator, will be in the build folder.

