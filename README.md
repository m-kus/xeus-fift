# Xeus-Fift
[![Build Status](https://travis-ci.org/atomex-me/xeus-fift.svg?branch=master)](https://travis-ci.org/atomex-me/xeus-fift)
[![Docker Build Status](https://img.shields.io/docker/cloud/build/atomex/xeus-fift)](https://hub.docker.com/r/atomex/xeus-fift)
[![made_with xeus](https://img.shields.io/badge/made_with-xeus-yellowgreen.svg)](https://github.com/QuantStack/xeus)
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/atomex-me/xeus-fift/binder?filepath=func_cheat_sheet.ipynb)

Jupyter kernels for the Fift and FunC languages (and TVM assembler) 

![Xeus-Fift](https://i.imgur.com/0UtNcmL.gif)

## Features

### Fift kernel
Aside from the convenient Jupyter interactive workflow this kernel provides:

* Syntax highlighting
* Words autocomplete via `Tab` (including those which are included or defined by you)
* In-place docstrings via `Shift+Tab` (for words defined in FiftBase and TVM pdf files)

### FunC kernel
* Syntax highlighting including (non)const methods and pseudo-namespaces
* Autoindentation
* Autocomplete by `Tab` using keywords and global functions
* Inspect function signature by `Shift+Tab`        
* Extra: `#include` macro and `print` debug helper

## FunC Workflow
1. At the top-level of each cell you can use `#include "path/to/file"` macro to load functions from a `.fc` file. They will be available throughout the notebook.
2. You can define multiple functions in any cell, they will also be available from everywhere.
3. At the end of the cell (using separate cell is recommended) you can write piece of code not wrapped by a function definition. It will be automatically used as `main` body. Such code cannot be accessed from another cell.
4. Alternatively, you can specify `main` function explicitly.
5. You can omit `return` and trailing `;` in the main code block - they will be added during the execution.
6. Note, that every time you re-run a cell, all functions defined in this cell are being overwritten.

### Examples
Return constant
```cpp
<<< 2 + 2
>>> 4
```

Return variable
```cpp
<<< int i = 42
>>> 42
```

Return function result
```cpp
<<< int sum(int a, int b) { return a + b; }
... sum(2, 2)
>>> 4
```

Print variables
```cpp
<<< int i = 1;
... int j = 2;
... print(i);
... print(j);
>>> 2 1
```

## How to use

### Run online!
Powered by awesome Binder: https://mybinder.org/v2/gh/atomex-me/xeus-fift/binder?filepath=func_cheat_sheet.ipynb

### Run in docker
0. Get the latest image from dockerhub (only when new releases are published)
```
docker pull atomex/xeus-fift
```
1. Create container using verified docker image:
```
docker run --rm -it -p 127.0.0.1:8888:8888 -v $(pwd):/home/jupyter/notebooks atomex/xeus-fift
```
2. Open the link from container output in your browser
3. Save notebooks in the mapped folder in order not to loose them

### Install .deb package
1. Check out the latest release tag at https://github.com/atomex-me/xeus-fift/releases
2. Download and install the package
```
wget https://github.com/atomex-me/xeus-fift/releases/download/0.1.0/xeus-fift_0.1.0-1_amd64.deb -P /tmp/
sudo dpkg -i /tmp/xeus-fift_0.1.0-1_amd64.deb
```
3. Check that Jupyter is now supporting Fift and FunC kernels
```
jupyter kernelspec list
```

### Install from sources
1. Ensure the following packages are installed: `libssl-dev zlib1g-dev uuid-dev`
2. Get the sources, build and install
```
git clone https://github.com/atomex-me/xeus-fift
cd xeus-fift
mkdir build
cd build
cmake ..
make xeus-fift
sudo make install
```
3. Check that Jupyter is now supporting Fift and FunC kernels
```
jupyter kernelspec list
```
