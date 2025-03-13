#!/usr/bin/env bash

trap "exit" INT

type=0
m=2
weight_factor=0.5

ns=(50 100 200 300 400 500 600 700 800 900 1000 1500 2000)
seeds=(1 2 3 4 5 6 7 8 9 10)

for n in ${ns[@]}; do
  for s in ${seeds[@]}; do
    ./../build/mobkp-instances --type=$type --n=$n --m=$m --seed=$s --weight-factor=$weight_factor
  done
done
