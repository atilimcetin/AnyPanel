#!/bin/bash

echo -n '8.8.8.8: '
ping -c 1 8.8.8.8 | awk -F ' |=' '/time/{print $10"ms"}'
