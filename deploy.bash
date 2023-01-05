#!/bin/bash
find . -type f -exec chmod 0644 '{}' \;
find . -type d -exec chmod 0755 '{}' \;
find . -maxdepth 1 -type f -name '*.bash' -exec chmod 0754 '{}' \;
rsync -aSP \
--exclude='.git' \
--exclude='*egg' \
--delete-after \
./ \
gg:usr/lib/e69/

