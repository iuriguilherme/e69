#!/bin/bash
rsync -aSP \
--exclude '.git'\
--exclude '*egg'\
./ \
gg:usr/lib/e69/
