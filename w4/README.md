# Workshop 4
## Installation
Get the unix source release
```shell
wget http://prdownloads.sourceforge.net/corona/corona-1.0.2.tar.gz?download ~/Downloads
cd ~/download && tar -xvf corona-1.0.2.tar.gz
```
Add the <cstring> include to files if necessary for memcpy commands and make the release
```shell
cd ~/Downloads/corona-1.0.2 && make && sudo make install
```
Add the install directory (/usr/local/lib) to your LD_LIBRARY_PATH if necessary.

## Run
In the w4 directory
```
make && ./w4 <image file path>
```