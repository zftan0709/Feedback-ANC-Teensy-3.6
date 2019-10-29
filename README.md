# **Feedback Active Noise Cancelling System**
---

[//]: # (Image References)

[image1]: ./README_image/feedbackANC.png "Feedback System"
[image2]: ./README_image/feedbackFXLMS.png "Feedback FxLMS"
[image3]: ./README_image/teensy.png "Teensy 3.6"
[image4]: ./README_image/max9744.jpg "Max9744"
[image5]: ./README_image/mic.jpg "INMP401"


## Introduction

In this project, Filtered-X Least Mean Square (FxLMS) algorithm is implemented in Teensy 3.6 microcontroller to reduce unwanted noise using a 20W speaker and MEMS microphone. 

## FxLMS Algorithm

This feedback active noise cancelling system is based on FxLMS algorithm. Since only the feedback signal is picked up, the feedback system is mostly effective in cancelling narrow-band signals.

![alt text][image1]

Figure 1: Physical Feedback System

![alt text][image2]

Figure 2: Feedback System based on FxLMS

In the feedback system, there is only error sensor(microphone) and a secondary source(speaker). The difference between the detected and predicted error is then feed into the system to update the filter weights.

---
## Physical Setup

### Microcontroller
The microcontroller used in this project is [Teensy 3.6]( https://www.pjrc.com/store/teensy36.html), a complete USB-based microcontroller development system by PJRC. With a 32 bit MHz ARM Cortex-M4 processor which comes with a floating point unit, it allows faster floating point computation.

![alt text][image3]

Figure 3: Teensy 3.6 Pinout

### Other Component

Audio amplifier: [Adafruit 20W MAX9744]( https://www.adafruit.com/product/1752) is selected for this project.

![alt text][image4]

Microphone: [SparkFun MEMS Microphone Breakout – INMP401](https://www.sparkfun.com/products/9868)

![alt text][image5]

## Code

The code is written using Arduino IDE with [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html). In the code, ARM math library is used to decrease the time used for floating point matrix calculation. DMA channel is also utilized in the code to achieve higher sampling frequency.

The secondary path file is first run to determine the secondary path coefficients offline. After acquiring the secondary path coefficients, they are then pasted into the main ANC file. With the acoustic path coefficients determined offline, the main file can now be run to reduce noise.

---
## Reference

Figures 1,2 are from paper by Edgar Lopez-Caudana

[Omar, Edgar. “Active Noise Cancellation: The Unwanted Signal and the Hybrid Solution.” Adaptive Filtering Applications, 2011, doi:10.5772/16844.]( https://www.researchgate.net/publication/221913037_Active_Noise_Cancellation_The_Unwanted_Signal_and_the_Hybrid_Solution)

Feedback FxLMS MATLAB code by Augustinus Oey

https://www.mathworks.com/matlabcentral/fileexchange/29491-feedback-active-noise-control-system-using-fblms-algorithm
