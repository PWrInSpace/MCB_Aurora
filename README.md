# MCB_Aurora


## Protobuf install


Clone protobuf from github repository
```
git clone https://github.com/protobuf-c/protobuf-c.git --branch "protobuf-c 1.4.1"
```

Instal packages
```
sudo apt-get install autoconf
sudo apt-get install automake
sudo apt-get install libtool
sudo apt-get install portobuf-devel
```
Install porotbuf
```
sudo ./autogen.sh
sudo ./configure
sudo make
sudo make install
```

Run porotobuf code generator xDDDDDDDDDDDDDDDDDDDD
Nie wiem jak to podpiąc pod tego cmake
```
cd components/protobuf/ && protoc --c_out=. lora.proto && cd .. && cd ..
```
