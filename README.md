# iLASH: Ultra-Rapid Detection of IBD Tracts

## Compiling

To compile iLASH, CMAKE v3.5 or higher is required.
First, create a directory for to generate the Makefile:

`$ mkdir build`

`$ cd build`

Generate the Makefile using the following command. C++ compiler used by CMAKE should support C++ 11 standard libraries (GCC v5.0 or newer is recommended).

`$ cmake ..`

Compile iLASH using the generated Makefile:

`$ make`

The compiled output named 'IBD' can be accessed from the build folder now.

## Running iLASH

iLASH requires a configuration file as input.

`$ IBD configuration_file_address`

A sample configuration file is available in the repository.

## Configuration File

iLASH is highly customizable. Some parameters of the configuration file require knowledge about the inner workings of iLASH, and some don't.
A detailed description of iLASH and its underlying algorithms is published at. Here we will go through all parameters. The ones that are more advanced and require reading the paper will be labeles with a star.

### Input/Output

- **ped:** Address to the phased PLINK PED formatted input.
- **map:** Address to the map file corresponding to the PED file. [PLINK PED/MAP format reference.](https://www.cog-genomics.org/plink/1.9/formats#ped)
- **output:** Output address. IBD tracts will be written to this file with the following format:

`FAMILY_ID1 SAMPLE_ID1 FAMILY_ID2 SAMPLE_ID2 CHROMOSOME_NUMBER START_BASEPAIR END_BASEPAIR START_SNPID END_SNPID LENGTH(in cM) ESTIMATED_SIMILARITY`

### Segments and Slices

iLASH divides the genotype data in consecutive slices and then runs LSH algorithm over each slice. Slices (windows) are, thus, the building blocks of IBD tracts. The following parameters control different properties of these slices.

![Slicing Schematic](./assets/slicing.png)

- **auto_slice:** This parameter controls the slicing approach. There are currently two approaches implemented. SNP-based and distance-based. Setting this parameter to 0 makes iLASH to use a SNP-based approach; Taking every *n* SNPs as a slice. Setting auto-slice to 1 would trigger distance-based slicing. This approach uses the genetic distances in the MAP file to decided the boundaries of each slice. Each slice will be as long as the minimum length parameter. Recommended value for this parameters is 1.
- **slice_size:** If *auto_slice* mode is set to *0*, This parameter would be used for SNP-based slicing. For example if this parameter is set to 2000, each slice will be 2000 SNPs long.
- **step_size:** In the SNP-based slicing approach, step size dictates how many SNPs to consecutive windows will *not* share. Figure below shows a scenario with a slice size of 2000 SNPs and a step size of 1000 SNPs.
![SNP-Based Slicing](./assets/snp_based.png)
- **min_length:** Minimum length of IBD tracts we are looking for. Segments that are shorter than this threshold will be ignored. If *auto_slice* mode is set to 1, this value will be used in conunction with the map file to determine the size of each slice.
- **cm_overlap:** If *auto_slice* mode is set to 1, this value sets the amount of overlap two consecutive windows share. It can be thought of as the inverse of *step_size* parameter for distance-based slicing. For example, if *min_length* is set to 3 cM and *cm_overlap* is set to one, as shown in the figure below, *min_length - cm_overlap (3-1=2)* will be used to determine the starting point for the next slice.
![Distance-Based Slicing](./assets/dist_based.png)

### LSH-Related Parameters

- **ped:**
- **ped:**
- **ped:**
- **ped:**
