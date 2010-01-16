#!/bin/sh
./autogen.sh && make distcheck && tar xf xiphos*tar.gz && cd xiphos-3.* && ./waf distcheck
