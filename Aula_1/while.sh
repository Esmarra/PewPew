#!/bin/sh
until [ ! $# -gt 0 ]
do
echo $1
shift
done
