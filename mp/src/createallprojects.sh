#!/bin/bash

pushd `dirname $0`
devtools/bin/vpc /hl2mp /hl2 /episodic +everything /mksln everything
popd