#!/bin/bash

echo const unsigned char doc_gzip[] = {
cat docs.txt | gzip -c | hexdump -ve '1/1 "0x%.2x, "'
echo }\;

echo const unsigned char inter_head[] = {
cat int_head.html | hexdump -ve '1/1 "0x%.2x, "'
echo }\;

echo const unsigned char inter_foot[] = {
cat int_foot.html | hexdump -ve '1/1 "0x%.2x, "'
echo }\;
