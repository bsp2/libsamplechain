# libsamplechain

A portable and platform agnostic C library to create sample chains, e.g. for the Elektron Analog Rytm (and Octatrack).

This library was created to consolidate a number of different sample chain implementations by different authors.

Currently available algorithms are:

### bsp_varichain

This algorithm creates a "variable sample chain" (varichain), where the sample slices are distributed irregularly.

The advantage of this is that the resulting chain is shorter than a sample chain that uses fixed-size slices where each slice is aligned to the maximum slice length.

Each sample slice can be padded with extra silence to avoid accidental triggering of the following slice. The sample "END" parameter on the Analog Rytm can then be set to 120 and only the "STA" parameter needs to be adjusted to select a slice.

There are actually two padding parameters:
* "extra_padding": sets the nominal padding
* "min_padding": sets the guaranteed minimum padding
