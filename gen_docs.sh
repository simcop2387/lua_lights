#!/bin/bash

echo const char doc_gzip[] = {
cat docs.txt | gzip -c | hexdump -ve '1/1 "0x%.2x, "'
echo }\;

echo const char inter_head[] = {
cat int_head.html | hexdump -ve '1/1 "0x%.2x, "'
echo 00}\;

echo const char inter_foot[] = {
cat int_foot.html | hexdump -ve '1/1 "0x%.2x, "'
echo 00}\;
