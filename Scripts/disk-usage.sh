#!/bin/bash
echo -n 'Disk usage: '
df / | grep -v 'Filesystem' | awk '{ print $5 }'
