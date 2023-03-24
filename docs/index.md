

# ROCm System Management Interface (ROCm SMI) Library

The ROCm System Management Interface Library, or ROCm SMI library, is part of the Radeon Open Compute [ROCm](https://github.com/RadeonOpenCompute) software stack . It is a C library for Linux that provides a user space interface for applications to monitor and control GPU applications.

## DISCLAIMER

The information contained herein is for informational purposes only, and is subject to change without notice. In addition, any stated support is planned and is also subject to change. While every precaution has been taken in the preparation of this document, it may contain technical inaccuracies, omissions and typographical errors, and AMD is under no obligation to update or otherwise correct this information. Advanced Micro Devices, Inc. makes no representations or warranties with respect to the accuracy or completeness of the contents of this document, and assumes no liability of any kind, including the implied warranties of noninfringement, merchantability or fitness for particular purposes, with respect to the operation or use of AMD hardware, software or other products described herein.

© 2022 Advanced Micro Devices, Inc. All Rights Reserved.


# Building ROCm SMI

#### Additional Required software for building
In order to build the ROCm SMI library, the following components are required. Note that the software versions listed are what was used in development. Earlier versions are not guaranteed to work:
* CMake (v3.5.0)
* g++ (5.4.0)

In order to build the latest documentation, the following are required:
* Python 3.8+
* NPM (sass)

The source code for ROCm SMI is available on [Github](https://github.com/RadeonOpenCompute/rocm_smi_lib).

After the ROCm SMI library git repository has been cloned to a local Linux machine, building the library is achieved by following the typical CMake build sequence. Specifically,
```shell
mkdir -p build
cd build
cmake <location of root of ROCm SMI library CMakeLists.txt>
make
# Install library file and header; default location is /opt/rocm
$ make install
```
The built library will appear in the `build` folder.

To build the rpm and deb packages follow the above steps with:
```shell
make package
```

#### Documentation
The following is an example of how to build the docs:
```shell
sudo apt install -y npm
sudo npm install -g sass

python3 -m venv .venv

.venv/bin/python3 -m pip install -r docs/.sphinx/requirements.txt
.venv/bin/python3 -m sphinx -T -E -b html -d docs/_build/doctrees -D language=en docs docs/_build/html
```

#### Building the Tests
In order to verify the build and capability of ROCm SMI on your system and to see an example of how ROCm SMI can be used, you may build and run the tests that are available in the repo. To build the tests, follow these steps:

```shell
# Set environment variables used in CMakeLists.txt file
ROCM_DIR=<parent dir. to lib/ and inc/, containing RSMI library and header>
mkdir <location for test build>
cd <location for test build>
cmake -DROCM_DIR=$ROCM_DIR <ROCm SMI source root>/tests/rocm_smi_test
make
```

To run the test, execute the program `rsmitst` that is built from the steps above.

# Usage Basics
## Device Indices
Many of the functions in the library take a "device index". The device index is a number greater than or equal to 0, and less than the number of devices detected, as determined by `rsmi_num_monitor_devices()`. The index is used to distinguish the detected devices from one another. It is important to note that a device may end up with a different index after a reboot, so an index should not be relied upon to be constant over reboots.

# Hello ROCm SMI
The only required ROCm-SMI call for any program that wants to use ROCm-SMI is the `rsmi_init()` call. This call initializes some internal data structures that will be used by subsequent ROCm-SMI calls. 

When ROCm-SMI is no longer being used, `rsmi_shut_down()` should be called. This provides a way to do any releasing of resources that ROCm-SMI may have held. In many cases, this may have no effect, but may be necessary in future versions of the library.

A simple "Hello World" type program that displays the device ID of detected devices would look like this:

```c
#include <stdint.h>
#include "rocm_smi/rocm_smi.h"
int main() {
  rsmi_status_t ret;
  uint32_t num_devices;
  uint16_t dev_id;

  // We will skip return code checks for this example, but it
  // is recommended to always check this as some calls may not
  // apply for some devices or ROCm releases

  ret = rsmi_init(0);
  ret = rsmi_num_monitor_devices(&num_devices);

  for (int i=0; i < num_devices; ++i) {
    ret = rsmi_dev_id_get(i, &dev_id);
    // dev_id holds the device ID of device i, upon a
    // successful call
  }
  ret = rsmi_shut_down();
  return 0;
}
```
