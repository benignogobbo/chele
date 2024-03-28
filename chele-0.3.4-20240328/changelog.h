// +---------------------------------------------------+
// | Dew Point sniffers pump currents monitor          |
// | Changelog                                         |
// | Benigno Gobbo                                     |
// | © 2024 Istituto Nazionale di Fisica Nucleare      |
// +---------------------------------------------------+
// Changelog
// 2023-05-04 Version 0.0.3 - First non-alpha release
// 2023-05-11 Version 0.0.4 - Moved from single to continuous ADC readout
// 2023-05-14 Version 0.0.5 - More info on OLED display
// 2023-05-18 Version 0.1.2 - First release
// 2023-05-19 Version 0.1.3 - Added display contrast setting and display on/off commands
// 2023-05-26 Version 0.1.4 - Minor changes in SD readout
// 2023-06-07 Version 0.1.5 - More changes in SD readout
// 2023-06-08 Version 0.1.6 - Added option to update config.txt file from Serial
// 2023-06-12 Version 0.1.7 - Added debug printout on Serial1
// 2023-06-18 Version 0.1.8 - Fixed Serial config.txt download
// 2023-11-10 Version 0.1.9 - Fixed NTP: JST is +9...
// 2023-12-06 Version 0.2.0 - Increase delay after net init to prevent fast switch on/off troubles
// 2024-02-26 Version 0.3.0 - Added getCurrent function to get both RMS method and MAX method values to speed up measurements
// 2024-02-27 Version 0.3.1 - Returns current time seconds too ( hh:mm -> hh:mm:ss )
// 2024-03-08 Version 0.3.2 - Small fixes to allow microSD update via Serial
// 2024-03-27 Version 0.3.3 - Server does not broadcast to all connected clients
// 2024-03-28 Version 0.3.4 - Added support to ILI9341 320x240 TFT display
#define VERSION_MAJOR 0
#define VERSION_MINOR 3
#define VERSION_REV   4
