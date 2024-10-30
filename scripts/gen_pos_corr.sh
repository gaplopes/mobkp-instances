#!/usr/bin/env bash

trap "exit" INT

ns=(50 100 200 300 400 500 600 700 800 900 1000 1500 2000)
seeds=(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)
correlations=(0.1 0.25 0.5 0.8)

type=2
m=2
weight_factor=0.5

for n in ${ns[@]}; do
  for s in ${seeds[@]}; do
    for c in ${correlations[@]}; do
      ./../build/mobkp-instances --type=$type --n=$n --m=$m --seed=$s --weight-factor=$weight_factor --correlation=$c
    done
  done
done
