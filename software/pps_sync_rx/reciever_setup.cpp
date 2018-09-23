#include <iostream>
#include "lime/LimeSuite.h"
#include "reciever_setup.h"

using namespace std;

/* SDR Device Structure */
lms_device_t* device = NULL;

/* Prototypes */
int open_reciever(void);


/* Configure Reciever */
int configure_reciever(reciever_configuration rx_config){
    
    /*  DEVICE SETUP  */
    
    /* Connect to LimeSDR */
    if (open_reciever() != 0)
        return -1;

    /* Initialize Device with Default Configuration */
    if (LMS_Init(device) != 0)
        error();

    /* Enable RX Channel */
    if (LMS_EnableChannel(device, LMS_CH_RX, 0, true) != 0)
        error();

    
    /*  LO SELECTION  */

    /* Set Centre Frequency */
    if (LMS_SetLOFrequency(device, LMS_CH_RX, 0, rx_config.centre_frequency) != 0)
        error();

    /* Print Selected Centre Frequency */
    float_type freq;
    if (LMS_GetLOFrequency(device, LMS_CH_RX, 0, &freq) != 0)
        error();
    cout << "\nCenter frequency: " << freq / 1e6 << " MHz\n";

    
    /*  ANTENNA SELECTION  */
    
    /* Get Avaliable Antennae */
    int num_ant;
    lms_name_t antenna_list[10];    
    if ((num_ant = LMS_GetAntennaList(device, LMS_CH_RX, 0, antenna_list)) < 0)
        error();

    /* Print Avaliable Antennae */
    cout << "Available antennae:\n";
    for (int i = 0; i < num_ant; i++)
        cout << i << ": " << antenna_list[i] << endl;

    /* Print Currently Selected Antenna */
    int ant_idx;
    if ((ant_idx = LMS_GetAntenna(device, LMS_CH_RX, 0)) < 0)
        error();
    cout << "Automatically selected antenna: " << ant_idx << ": " << antenna_list[ant_idx] << endl;

    /* Select Desired Antenna */
    if (LMS_SetAntenna(device, LMS_CH_RX, 0, rx_config.antenna) != 0)
        error();

    /* Print Currently Selected Antenna */
    if ((ant_idx = LMS_GetAntenna(device, LMS_CH_RX, 0)) < 0)
        error();
    cout << "Manually selected antenna: " << ant_idx << ": " << antenna_list[ant_idx] << endl;


    /*  SAMPLE RATE SELECTION  */

    /* Set Sample Rate & Preferred Oversampling in RF */
    if (LMS_SetSampleRate(device, rx_config.sample_rate, rx_config.oversample_ratio) != 0)
        error();
    
    /* Print Resulting Sampling Rates (ADC & Host Interface) */
    float_type rate, rf_rate;
    if (LMS_GetSampleRate(device, LMS_CH_RX, 0, &rate, &rf_rate) != 0)
        error();
    cout << "Host interface sample rate: " << rate / 1e6 << " MHz\nRF ADC sample rate: " << rf_rate / 1e6 << "MHz\n";


    /*  LPF SELECTION  */

    /* Print LPF Allowed Range */
    lms_range_t range;
    if (LMS_GetLPFBWRange(device, LMS_CH_RX, &range)!=0)
        error();
    cout << "RX LPF bandwitdh range: " << range.min / 1e6 << " - " << range.max / 1e6 << " MHz\n";
    
    /* Enable Analog RX LPF & Set Bandwidth */
    if (LMS_SetLPFBW(device, LMS_CH_RX, 0, rx_config.LPF_bandwidth) != 0)
        error();
    cout << "RX LPF bandwitdh: " <<  rx_config.LPF_bandwidth / 1e6 << " MHz\n";


    /*  RX GAIN SELECTION  */

    /* Set RX Gain - 0 to 1.0 */
    if (LMS_SetNormalizedGain(device, LMS_CH_RX, 0, rx_config.rx_gain) != 0)
        error();

    /* Print Normalised RX Gain */
    float_type gain;
    if (LMS_GetNormalizedGain(device, LMS_CH_RX, 0, &gain) != 0)
        error();
    cout << "Normalized RX Gain: " << gain << endl;

     /* Print Resulting RX Gain in dB */
    unsigned int gaindB;
    if (LMS_GetGaindB(device, LMS_CH_RX, 0, &gaindB) != 0)
        error();
    cout << "RX Gain: " << gaindB << " dB" << endl;


    /*  CALIBRATION  */

    /* Perform Automatic Calibration */
    if (LMS_Calibrate(device, LMS_CH_RX, 0, rx_config.cal_bandwidth, 0) != 0)
        error();

    /* Return Success */
    return 0;
}


/* Open LimeSDR Device */
int open_reciever(void){

    /* Find Number of Devices Attached */
    int num_dev;
    if ((num_dev = LMS_GetDeviceList(NULL)) < 0)
        error();
    cout << "Devices found: " << num_dev << endl;
    if (num_dev < 1)
        return -1;

    /* Allocate & Populate Device List */
    lms_info_str_t* list = new lms_info_str_t[num_dev];   
    if (LMS_GetDeviceList(list) < 0)                
        error();

    /* Print out Device List */
    for (int i = 0; i < num_dev; i++)                     
        cout << i << ": " << list[i] << endl;
    cout << endl;

    /* Open the First Device */
    if (LMS_Open(&device, list[0], NULL))
        error();

    /* Delete List */
    delete [] list;
    return 0;
}


/* Error Handler */
int error(){
    if (device != NULL)
        LMS_Close(device);
    exit(-1);
}