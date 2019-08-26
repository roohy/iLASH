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

As mentioned before, iLASH performs a realization of LSH algorithm (Inspired from [Mining of Massive Datasets, Chapter 3](http://www.mmds.org)) on each slice along the genome to find, for each individual haplotype, other haplotypes that are similar to it on the same slice (window) along the chromosome. The criteria used for measuring this similarity is [Jaccard Similarity](https://en.wikipedia.org/wiki/Jaccard_index) among pairs of haplotypes. For every haplotype at a slice, contigous markers are groupd together to make shingles(tokens, k-mers, or words). This will turn every haplotype (at a slice) to a set of words and enable Jaccard Index comparisons. To learn more about LSH, Minhashing and other details regarding this step, either the paper or [Mining of Massive Datasets, Chapter 3](http://www.mmds.org) can be useful. Here we just explain the parameters that help control the various properties of the algorithm and will not go into the details of the algorithm itself.

- **shingle_size:** This parameter controls the size of every shingle (k-mer, token or word) in terms of number of SNPs in each one. Larger shingles, will contribute to less accruacy and shorter shingle can lead to more false-positives and/or slower performance of iLASH. We recommend choosing a shingle size of 15-30.
- **shingle_overlap:** Two neighboring shingles can overlap. This parameters controls that. We recommend an overlap of zero between shingles as it can lead to false-positives.
- **perm_count:** Number of permutations done for the *Minhashing* step of LSH. Increasing the number of permutations will cause iLASH to run slower. However, The more permutations done in minhashing, the more accurate the results. We recommend 20 permutations for a subtle accuracy and 12 permutations for a speedy performance.
- **bucket_count:** Number buckets (LSH values) for generating LSH signature. *perm_count* value should be divisible by *bucket_count*. We recommend having a bucket count of 4 with 20 permutations and a bucket count of 3 with 12 permutations.
- **interest_threshold:** Minimum estimated similarity score between 2 slices in order for them to be declared a possible match pair. Possible matches are further investigated in detail to find IBD segments.
- **match_threshold:** Minimum estimated similarity score between 2 slices in order for them to be declared a matched pair. Matched pair are considered a match without any further detailed analysis. This can speed of the algorithm in exchange for possible false-positives.
- **minhash_threshold:** When *auto_slice* is set to 1 (or anything other than 0), This parameter sets the minimum length of a slice in terms of SNPs. If a slice has fewer SNPs comprising them, they will not be analyzed in LSH steps. This is to prevent false-positives in areas of low-complexity on the genome, also any other places with SNP density of the array so low that the small number of total possible slice values.

### Other Parameters

- **max_thread:** This parameters sets the maximum number of threads to be requested by iLASH. We recommend setting it to zero on non-cluster machines so that iLASH could request for the optimal number itself.
- **max_error:** This is item is not fully implemented yet.
