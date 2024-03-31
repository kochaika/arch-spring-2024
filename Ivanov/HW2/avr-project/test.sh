#!/bin/bash

make run > out
cat ./out | head -n 14 | tail -n 12 > out_parsed
DIFF=$(diff ./correct_out ./out_parsed)
if [ "$DIFF" != "" ]
then
  echo "Error happened. Diff:"
  echo "$DIFF"
else
  echo "Ok."
fi
