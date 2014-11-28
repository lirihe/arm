Compression / Decompression
===========================

The gomspace library includes a compression library called `minilzo` written by Markus F.X.J. Oberhumer, [oberhumer.com](http://www.oberhumer.com/opensource/lzo/). This library supports compression of raw LZO data blocks alone, so it can only be used for blocks up to 256 kB and all the meta-data needs to be added manually.

Therefore Gomspace has reverse engieneered the opensource program `lzop` and added utility fucntions `lzoutils.c` for handling data in the lzop package format. This means that files compressed using the gomspace `lzoutils` can be decompressed on linux/windows/mac with the `lzop` program, or any other compressed file handler, and vice versa for decompression.

