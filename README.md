## signal-generator ##

Generates data file for a fake signal module for HCAL uHTR

### How to Use ###

A basic use case is given in main.cpp. 

*  **Constructor** - Default constructor creates 1 fiber. Passing one integer sets the number of fibers.
* **get\_data** -- Overide this function to create whatever data you like based on fiber, bunch crossing and qie/channel number.
