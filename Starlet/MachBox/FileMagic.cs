using System;

namespace Starlet
{
    /*
     * iOS (and all of Apple's arm based devices) run using little endian
     * That means that if we wanna run a binary built for an iOS device
     * We should reference FileCigamHex
     *
     * Also I removed the armv8 stuff because if you look at iOS-Devices-Simplified
     * you can see that arm64 == armv8 lol [no need to have more stuff ;P]
     */
    class FileCigamHex
    {
        public static string hex_fat_bin_cigam = "BE BA FE CA"; //hex for a mach-o that's f a t
        
        public static string hex_armv6_cigam = "";
        public static string hex_armv7_cigam = "";
        public static string hex_armv7s_cigam = "";
        //public static string hex_armv8_cigam = "";
        public static string hex_arm64_cigam = "";
        
    }
}