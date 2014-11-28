#!/bin/bash

cat docs.txt | gzip -c | hexdump -ve '1/1 "0x%.2x, "'
