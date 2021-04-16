#!/usr/bin/env bash

for i in $(seq 1 12); do
  ./program.out data/saj"$i".txt > output/saj"$i".txt
done
