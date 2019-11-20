# Xeus-Fift
[![Build Status](https://travis-ci.org/atomex-me/xeus-fift.svg?branch=master)](https://travis-ci.org/atomex-me/xeus-fift)
[![Docker Build Status](https://img.shields.io/docker/cloud/build/atomex/xeus-fift)](https://hub.docker.com/r/atomex/xeus-fift)
[![made_with xeus](https://img.shields.io/badge/made_with-xeus-yellowgreen.svg)](https://github.com/QuantStack/xeus)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Jupyter kernel for the Fift language (and TVM assembler)

![Xeus-Fift](https://i.imgur.com/0UtNcmL.gif)

## Features
Aside from the convenient Jupyter interactive workflow this kernel provides:

* Syntax highlighting
* Words autocomplete via `Tab` (including those which are included or defined by you)
* In-place docstrings via `Shift+Tab` (for words defined in FiftBase and TVM pdf files)

## How to use

### Run in docker
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
3. Check that Jupyter is now supporting the Fift kernel
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
3. Check that Jupyter is now supporting the Fift kernel
```
jupyter kernelspec list
```
