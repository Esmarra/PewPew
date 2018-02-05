#!/bin/sh
#ls $1
echo $(($(date -d "$(date +00:00-24:00)" +%M)-$(date +%M)))
