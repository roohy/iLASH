#!/usr/bin/env bash


#wget http://www.cs.columbia.edu/~gusev/germline/germline-1-5-1.tar.gz

#tar -xvzf germline-1-5-1.tar.gz

sudo apt-get update

sudo apt-get install cmake

sudo apt-get install g++

wget roohy.me/HM3_chr1.tgz

wget roohy.me/maker.tar.gz

wget roohy.me/ped.10000.tar.gz
tar -zxvf makers.tar.gz

tar -zxvf HM3_chr1.tgz


./hapgen2 -m genetic_map_chr1_combined_b36.txt -Ne 11418 -h CEU+TSI.chr1.hap -l hapmap3.r2.b36.chr1.legend -dl 554484 1 1 1 -n 10000 -o test.10000

