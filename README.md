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

- **ped:** Address to the phased PLINK PED formatted input.
- **map:** Address to the map file corresponding to the PED file. [PLINK PED/MAP format reference.](https://www.cog-genomics.org/plink/1.9/formats#ped)
- **output:** Output address. IBD tracts will be written to this file with the following format:

`FAMILY_ID1 SAMPLE_ID1 FAMILY_ID2 SAMPLE_ID2 CHROMOSOME_NUMBER START_BASEPAIR END_BASEPAIR START_SNPID END_SNPID LENGTH(in cM) ESTIMATED_SIMILARITY`

- **auto_slice:** Slices (windows) are the building blocks of IBD tracts. This parameter controls the slicing approach. There are currently two approaches implemented. SNP-based and distance_based. Setting this parameter to 0 makes iLASH to use a SNP-based approach; Taking every *n* SNPs as a slice. Setting auto-slice to 1 would trigger distance-based slicing. This approach uses the genetic distances in the MAP file to decided the boundaries of each slice. Each slice will be as long as the minimum length parameter.
- **slice_size:** If *auto_slice* parameter is set to *0*, This parameter would be used for SNP-based slicing. For example if this parameter is set to 2000, each slice will be 2000 SNPs long.
- **step_size:** In the SNP-based slicing approach, step size dictates how many SNPs to consecutive windows will *not* share. See the figure below.
![SNP-Based Slicing](./assets/snp_based.png)
- **min_length:** Minimum length 
- **ped:**
- **ped:**
- **ped:**
- **ped:**
- **ped:**
