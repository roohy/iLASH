# iLASH: Ultra-Rapid Detection of IBD Tracts

## Compiling

To compile iLASH, CMAKE v3.5 or higher is required.
First, create a directory to generate the Makefile:

`$ mkdir build`

`$ cd build`

Generate the Makefile using the following command. C++ compiler used by CMAKE should support C++ 11 standard libraries (GCC v5.0 or newer is recommended).

`$ cmake ..`

Compile iLASH using the generated Makefile:

`$ make`

The compiled output named 'IBD' can be accessed from the *build* folder now.

## Running iLASH

iLASH requires a configuration file as input.

`$ IBD configuration_file_address`

A sample configuration file is available in the repository.

## Configuration File

iLASH is highly customizable. Some parameters of the configuration file require knowledge about the inner workings of iLASH, and some do not.
A detailed description of iLASH and its underlying algorithms can be found at. Here we will go through all parameters. The ones that are more advanced and require reading the paper will be labeled with a star.

### Input/Output

- **ped:** Address to the phased PLINK PED formatted input.
- **map:** Address to the map file corresponding to the PED file. [PLINK PED/MAP format reference.](https://www.cog-genomics.org/plink/1.9/formats#ped)
- **output:** Output address. IBD tracts will be written to this file with the following format:

`FAMILY_ID1 SAMPLE_ID1 FAMILY_ID2 SAMPLE_ID2 CHROMOSOME_NUMBER START_BASEPAIR END_BASEPAIR START_SNPID END_SNPID LENGTH(in cM) ESTIMATED_SIMILARITY`

### Segments and Slices

iLASH divides the genotype data in consecutive slices and then runs LSH algorithm over each slice. Slices (windows) are, thus, the building blocks of IBD tracts. The following parameters control different properties of these slices.

![Slicing Schematic](./assets/slicing.png)

- **auto_slice:** This parameter controls the slicing approach. There are currently two approaches implemented. SNP-based and distance-based. Setting this parameter to 0 orders iLASH to use a SNP-based approach; Taking every *n* SNPs as a slice. Setting auto-slice to 1 would trigger distance-based slicing. This approach uses genetic distances in the MAP file to decide the boundaries of each slice. Each slice will be as long as the minimum length parameter. Recommended value for this parameters is 1.
- **slice_size:** If *auto_slice* mode is set to *0*, This parameter would be used as slice length for SNP-based slicing. For example if this parameter is set to 2000, each slice will be 2000 SNPs long. This parameter will be ignored in other slice modes.
- **step_size:** In the SNP-based slicing approach, step size dictates how many SNPs two consecutive windows will *not* share. Figure below demonstrates a scenario with a slice size of 2000 SNPs and a step size of 1000 SNPs. This parameter is also ignored in other slicing modes.
![SNP-Based Slicing](./assets/snp_based.png)
- **min_length:** Minimum length of IBD tracts iLASH is looking for. Segments that are shorter than this threshold will be ignored. If *auto_slice* mode is set to 1, this value will be used in conjunction with the map file to determine the size of each slice in terms of number SNPs. This each slice will have a different SNP count but covers the same genetic distance. See the figure below.
- **cm_overlap:** If *auto_slice* mode is set to 1, this value sets the amount of overlap two consecutive windows share (in terms of genetic distance). It can be thought of as the inverse of *step_size* parameter for distance-based slicing. For example, if *min_length* is set to 3 cM and *cm_overlap* is set to 1 cM, as shown in the figure below, *min_length - cm_overlap (3-1=2)* will be used to determine the starting point for the next slice.
![Distance-Based Slicing](./assets/dist_based.png)

### LSH-Related Parameters

As mentioned before, iLASH performs a realization of LSH algorithm (Inspired from [Mining of Massive Datasets, Chapter 3](http://www.mmds.org)) on each slice along the genome to find, for each individual haplotype, other haplotypes that are similar to it on the same slice (window) along the chromosome. The criteria used for measuring similarity is [Jaccard Index](https://en.wikipedia.org/wiki/Jaccard_index) among pairs of haplotypes. For every haplotype at a slice, contiguous markers are grouped together to make shingles(tokens, k-mers, or words). This will turn every slice of every haplotype to a set of words and enable Jaccard Index comparisons. To learn more about LSH, Minhashing and other details regarding this step, both the paper and [Mining of Massive Datasets, Chapter 3](http://www.mmds.org) are useful to read. Here we will only explain the parameters that help control the various properties of the algorithms and will not go into the details of the algorithm itself.

- **shingle_size\*:** This parameter controls the size of every shingle (k-mer, token or word) in terms of number of SNPs in each one. Larger shingles, will contribute to less accruacy (but higher speed) and shorter shingle can lead to more false-positives and/or slower performance of iLASH. We recommend choosing a shingle size of 15-30.
- **shingle_overlap:** Two neighboring shingles can overlap. This parameters controls that. We recommend an overlap of zero between shingles as it can lead to false-positives.
- **perm_count\*:** Number of permutations done for the *Minhashing* step of LSH. Increasing the number of permutations will cause iLASH to run slower. However, The more permutations done in minhashing, the more accurate the results. We recommend 20 permutations for a subtle accuracy and 12 permutations for a speedy performance.
- **bucket_count\*:** Number of buckets (LSH values) for generating LSH signatures. *perm_count* value should be divisible by *bucket_count* value. We recommend having a bucket count of 4 with 20 permutations and a bucket count of 3 with 12 permutations.
- **interest_threshold:** Minimum estimated similarity score between two slices in order for them to be declared a possible match pair. Possible matche pairs are further investigated in detail to find if they contain IBD segments.
- **match_threshold:** Minimum estimated similarity score between two slices in order for them to be declared a matched pair. Matched pairs are considered to be an exact match without any detailed analysis. This can increase the speed of the algorithm in exchange for possible (although unlikely) false-positives.
- **minhash_threshold:** When *auto_slice* is set to 1 (or anything other than 0), This parameter sets the minimum possible length for a slice in terms of SNPs. If a slice has a smaller SNP count, they will not be analyzed in LSH steps. This is to prevent false-positives in areas of low-complexity on the genome, and any other places with SNP density of the array so low that the small number of total possible slice values. These areas will still be explored to extend IBD segments if their neighboring slices are declared a possible IBD match between two samples.

### Other Parameters

- **max_thread:** This parameter sets the maximum number of threads to be requested by iLASH. We recommend setting it to zero on non-cluster machines so that iLASH could request for the optimal number itself.
- **max_error:** This is item is not fully implemented yet.
