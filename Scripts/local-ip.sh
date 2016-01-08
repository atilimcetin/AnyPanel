#!/bin/bash

echo -n 'Local IP: '
ifconfig | grep inet | grep -v inet6 | grep -v '127.0.0.1' | cut -d ' ' -f2
