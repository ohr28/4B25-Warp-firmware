/*
accel_test.c contains the functions required for the height estimation and uncertainty detection;
*/


#include <stdlib.h>
#include "config.h"

#include "fsl_misc_utilities.h"
#include "fsl_device_registers.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_spi_master_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_mcglite_hal.h"    
#include "fsl_port_hal.h"

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"
#include "accel_test.h"

//#include "devMMA8451Q.h"

void getXReadings(int x, int delay) {

    // Generate X readings with some required delay;

    for (int i=0; i<x; i++) {
        printSensorDataMMA8451Q(0);
        warpPrint("\n");
        
        // DEBUG
        // uint8_t flags = readSensorRegisterMMA8451Q(0x16, 1);
	    //warpPrint("Flag: %d\t", flags);

        OSA_TimeDelay(delay);
    }
}

int16_t maxValue(int16_t* arr, int length) {

    // Return the magnitude of the maximum value in an array;

    int16_t maxVal = -8200;
    for (int i=0; i<length; i++) {
        if (arr[i] > maxVal) {
            maxVal = arr[i];
        }
    }
    return maxVal;
}

int16_t minValue(int16_t* arr, int length) {

    // Return the magnitude of the minimum value in an array;

    int16_t minVal = 8200;
    for (int i=0; i<length; i++) {
        if (arr[i] < minVal) {
            minVal = arr[i];
        }
    }
    return minVal;
}

int avgValue(int16_t* arr, int length) {

    // Return the average value of all the elements in an array;

    int avgValue = 0;
    for (int i=0; i<length; i++) {
        avgValue += arr[i];
    }
    return avgValue / length;
}

void integrate(int16_t* arr, int16_t* int_arr, int length) {

    // First integral produces velocity data from acceleration raw;

    // Assume zero;
    int_arr[0] = 0;

    for (int i=1; i<length; i++) {
        int_arr[i] = ((arr[i] + arr[i-1]) / 2) + int_arr[i-1];
    }
}

void double_integrate(int16_t* int_arr, int16_t* int_int_arr, int length) {

    // Second integral produces displacement data from computed velocity;

    // Assume zero;
    int_int_arr[0] = 0;
    int_int_arr[1] = 0;

    for (int i=2; i<length; i++) {
        int_int_arr[i] = ((int_arr[i] + int_arr[i-1]) / 2) + int_int_arr[i-1];
    }
}

void zeroOffset(int16_t* arr, int length) {

    // Offset an array by its average value (typically g in the case of the z-axis);

    int avgValueArr = avgValue(arr, length);
    for (int i=0; i<length; i++) {
        arr[i] -= avgValueArr;
    }
}

void measureArr(int16_t* arr, int length, int delay) {

    // Fill an array with z-axis acceleration data with an artificial delay;

    for (int i=0; i<length; i++) {
        arr[i] = obtainDirectionZ();
        OSA_TimeDelay(delay);
    }
}

void printArr(int16_t* any_arr, int length) {

    // Print out an array to the terminal in .csv-friendly format;

    warpPrint("Printing array:\n");
    for (int i=0; i<length; i++) {
        // warpPrint("%d: Z--> %d\n", i, any_arr[i]);
        warpPrint("%d\n", any_arr[i]);
    }
    warpPrint("Done!\n");
}

bool detectUp(int16_t* int_arr, int length, int bound) {

    // DEBUG
    // A debug function used for detecting positive violation of a vecloity threshold;

    int avgValueArr = avgValue(int_arr, length);
    int16_t upper = avgValueArr + bound;
    int16_t lower = avgValueArr - bound;
    int count = 0;
    for (int i=0; i<length; i++) {
        if (int_arr[i] >= upper) {
            count++;
        }
    }
    if (count > 0) {
        return true;
    } else {return false;}
}

bool detectDown(int16_t* int_arr, int length, int bound) {

    // DEBUG
    // A debug function used for detecting negative violation of a velocity threshold;
    int avgValueArr = avgValue(int_arr, length);
    int16_t upper = avgValueArr + bound;
    int16_t lower = avgValueArr - bound;
    int count = 0;
    for (int i=0; i<length; i++) {
        if (int_arr[i] <= lower) {
            count++;
        }
    }
    if (count > 0) {
        return true;
    } else {return false;}
}


int approxCI(int dist) {
    
    // Approximate the mass of a gaussian distribtuion based on std. deviations from the mean;
    // Differences are multiplied by 1000 for ease of manipulation;

    if (dist < 0) {
        dist *= -1;
    }

    // Gaussian integrals taken from the computed Z-score table;

    if (dist >=0 && dist <= 100) {
        return 54;
    } else if (dist >=100 && dist <= 200) {
        return 58;
    } else if (dist >=200 && dist <= 300) {
        return 62;
    } else if (dist >=300 && dist <= 400) {
        return 66;
    } else if (dist >=400 && dist <= 500) {
        return 69; 
    } else if (dist > 500 && dist <= 1000) {
        return 84;
    } else if (dist > 1000 && dist <= 1500) {
        return 93;
    } else if (dist >1500 && dist <= 1750) {
        return 96;
    } else if (dist > 1750 && dist <= 2000) {
        return 97;
    } else if (dist > 2000) {
        return 99;
    }
}

void probFloors(int settle, int mean_level, int std_dev_level, const int lowest, const int highest) {

    // DEBUG
    // Debug function for determining which floor is the closest to the true value measured;


    int min_offset_pos = 1000000;
    int pos_level = 0;
    int min_offset_neg = 1000000;
    int neg_level = 0;

    for (int level=lowest; level<=highest; level++) {
        int theoretical_level_value = level * mean_level;
        
        int pos_off = settle - theoretical_level_value;
        int neg_off = theoretical_level_value - settle;

        if (pos_off > 0) {
            if (pos_off < min_offset_pos) {
                min_offset_pos = pos_off;
                pos_level = level;
            }
        } else {
            if (neg_off < min_offset_neg) {
                min_offset_neg = neg_off;
                neg_level = level;
            }
        }


    }

    // min_offset_neg *= -1;
    int closest = min(min_offset_pos, min_offset_neg);
    int true_level = 0;

    if (min_offset_pos < min_offset_neg) {
        true_level = pos_level;
    } else {
        true_level = neg_level;
    }

    warpPrint("\n\nCLOSEST FLOOR: %d, %d, offset: %d %d, closest: %d, true: %d\n", pos_level, neg_level, min_offset_pos, min_offset_neg, closest, true_level);

}

void calcProb(int settle, int mean_level, int std_dev_level, const int lowest, const int highest) {

    // Function calculates the likelihood a given final measurement is closer to the centre of mass of any of
    // the individual floor distribution means;

    int numFloors = highest - lowest + 1;


    for (int level=lowest; level<=highest; level++) {
        
        int theoretical_level_value = level * mean_level;
        int std_dev = std_dev_level * level * level;

        int offset = ((settle - theoretical_level_value) * 1000) / std_dev;

        int ci = 100 - approxCI(offset);

        warpPrint("LEVEL %d: CI approx %d%%\n", level, ci);
    
    }

}

// DEBUG 
/*
void getFloorDisplacement(int16_t* int_int_arr, int length) {
    int i = 0;
    int j = length - 1;

    warpPrint("J value is: %d", j);

    int range = maxValue(int_int_arr, length) - minValue(int_int_arr, length);
    int bottomBound = (range / 10) + minValue(int_int_arr, length);
    int topBound = (range / 10) - maxValue(int_int_arr, length);

    warpPrint("TB is: %d", topBound);
    warpPrint("RA is: %d", range);

    while(int_int_arr[j] > topBound && j >= 0 ) {
        j--;
         
    }

    warpPrint("J value is: %d", j);
    while(j<length) {
        warpPrint("%d\n", int_int_arr[j]);
        j++;
    }
    
}
*/





void measureAndRun(int16_t* arr, int16_t* int_arr, int length, int delay, int bound, int* floorLevel) {
    
    // Main floor detection loop;

    warpPrint("\nSTARTING:\n");
	//OSA_TimeDelay(2000);
	measureArr(arr, length, delay);
	//warpPrint("ENDING REC:\n");
	//printArr(arr, length);

	int average = avgValue(arr, length);
	//warpPrint("AVERAGE -> %d\n", average);
	
	zeroOffset(arr, length);
	// printArr(arr, length);
	
	
	integrate(arr, int_arr, length);
	
    // Testing new bit:
    // printArr(int_arr, length);
    double_integrate(int_arr, arr, length);

    // printArr(arr, length);
    // printArr(int_arr, length);
    // printArr(arr, length);
    
    // getFloorDisplacement(int_int_arr, length);
    // printArr(int_int_arr, length);
    // warpPrint("DI Floors Detected: %d\n", detectFloors(int_int_arr, length, 10000));
	

    // DEBUG
    // Basic testing routines
    /*
	bool resultUP = detectUp(int_arr, length, bound);
	if (resultUP == 1) {
		warpPrint("GOING UP A FLOOR!\n");
        *floorLevel += 1;
	} else {
		warpPrint("NOTHING UP FOUND!\n");
        bool resultDOWN = detectDown(int_arr, length, bound);
        if (resultDOWN == 1) {
            warpPrint("GOING DOWN A FLOOR!\n");
            *floorLevel -= 1;
        } else {
            warpPrint("NOTHING DOWN FOUND!\n");
        }
	}
    warpPrint("FLOOR LEVEL: %d\n", *floorLevel);
    */

    
    
    

    
}






