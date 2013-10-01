## signal-generator ##

Generates data file for a fake signal module for HCAL uHTR

### How to Use ###

A basic use case is given in main.cpp. 

*  **Constructor** - Default constructor creates 1 fiber. Passing one integer sets the number of fibers.
* **`get\_data()`** - Overide this function to create whatever data you like based on fiber, bunch crossing and qie/channel number.
*  **Output files** - The `write_hex()` is the main function to call to create your data stream files. It takes a string as a parameter which must contain `"%02u"` to allow the creating of different files. In the example `"file%02u.txt"` creates `file00.txt`, `file01.txt`, etc.
