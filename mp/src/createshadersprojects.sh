#!/bin/bash

pushd `dirname $0`
devtools/bin/vpc /hl2mp /hl2mp /hl2 /episodic +shaders /mksln shaders
popd