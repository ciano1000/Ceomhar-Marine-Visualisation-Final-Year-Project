INTERNAL void GenerateCRC32Table() {
    
    u32 n,c;
    for(n = 0; n < 256; ++n) {
        c = n;
        // NOTE(Cian): Loop through each bit
        for(u32 k = 0; k < 8; ++k) {
            // NOTE(Cian): If the MSB is 1
            if(c & 1) {
                c = HASH_POLY ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        crc32_lut[n] = c;
    }
}

INTERNAL u32 StringToCRC32(char *string, u32 n) {
    u32 crc = 0;
    for(u32 i = 0; i < n; ++i) {
        crc = crc32_lut[(crc ^ string[n]) & 255] ^ (crc >> 8);
    }
}

INTERNAL u32 StringToCRC32(char *string) {
    StringToCRC32(string, strlen(string));
}