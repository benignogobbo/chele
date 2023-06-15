// +---------------------------------------------------+
// | Currents readout                                  |
// | Benigno Gobbo                                     |
// | © 2023 Istituto Nazionale di Fisica Nucleare      |
// +---------------------------------------------------+
#ifndef CURRENT_H
#define CURRENT_H

#include <Adafruit_ADS1X15.h>  // From https://github.com/adafruit/Adafruit_ADS1X15

const double _factor = 10.0; // 10A/1V

const int ALRT0 = 2, ALRT1 = 3;

Adafruit_ADS1115 ads0, ads1;

volatile bool newData0 = false;
void NewDataReadyISR0() {
  newData0 = true;
}

volatile bool newData1 = false;
void NewDataReadyISR1() {
  newData1 = true;
}

class Current {
 public:
  Current( void ){}
  void init( void );
  double getCurrentRMS( int );
  double getCurrentMOS( int );
};

void Current::init( void ) {

  pinMode( ALRT0, INPUT );
  pinMode( ALRT1, INPUT );

  attachInterrupt( digitalPinToInterrupt( ALRT0 ), NewDataReadyISR0, FALLING );
  attachInterrupt( digitalPinToInterrupt( ALRT1 ), NewDataReadyISR1, FALLING );

  ads0.setGain( GAIN_TWO );  // +- 2.048V 1 bit = 0.0625 mV
  ads1.setGain( GAIN_TWO );
  ads0.setDataRate( RATE_ADS1115_250SPS );
  ads1.setDataRate( RATE_ADS1115_250SPS);
  if( !ads0.begin( ADS1X15_ADDRESS ) ) {
    DBPRINTLN( "Failed ADS1115-0 initialization" );
    display.println( "Failed ADC-0 init." );
    display.display();
    while(1);
  }
  if( !ads1.begin( ADS1X15_ADDRESS + 1 ) ) {
    DBPRINTLN( "Failed ADS1115-1 initialization" );
    display.println( "Failed ADC-1 init." );
    display.display();
    while(1);    
  }
}

double Current::getCurrentRMS( int adc ) {

  double voltage, current, sum = 0;
  long time = millis();
  int counter = 0;

  switch( adc ) {
    case 0:
     ads0.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_0_1, true );
     break;
   case 1:
     ads0.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_2_3, true );
     break;
   case 2:
     ads1.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_0_1, true );
     break;
   case 3:
     ads1.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_2_3, true );
     break;
   default:
     DBPRINTLN( "ADS must be a number between 0 and 3!" );
     while( true );
     break;
  }

  while( millis() - time < 1000 ) {
    switch( adc ) {
      case 0:
        while( !newData0 );
        voltage = ads0.computeVolts( ads0.getLastConversionResults() );
        newData0 = false;
        break;
      case 1:
        while( !newData0 );
        voltage = ads0.computeVolts( ads0.getLastConversionResults() );
        newData0 = false;
        break;
      case 2:
        while( !newData1 );
        voltage = ads1.computeVolts( ads1.getLastConversionResults() );
        newData1 = false;
        break;
      case 3:
        while( !newData1 );
        voltage = ads1.computeVolts( ads1.getLastConversionResults() );
        newData1 = false;
        break;
      default:
        DBPRINTLN( "ADS must be a number between 0 and 3!" );
        while( true );
        break;
    }
    
    current = voltage * _factor;
    sum += sq( current );
    counter += 1;
  }

  current = sqrt( sum / counter );

  switch( adc ) {
    case 0:
     ads0.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_0_1, false );
     break;
   case 1:
     ads0.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_2_3, false );
     break;
   case 2:
     ads1.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_0_1, false );
     break;
   case 3:
     ads1.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_2_3, false );
     break;
   default:
     DBPRINTLN( "ADS must be a number between 0 and 3!" );
     while( true );
     break;
  }

  return( current );
}

double Current::getCurrentMOS( int adc ) {

  long time = millis();
  double voltage = 0, maxVolt = 0, minVolt = 0;

  switch( adc ) {
    case 0:
     ads0.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_0_1, true );
     break;
   case 1:
     ads0.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_2_3, true );
     break;
   case 2:
     ads1.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_0_1, true );
     break;
   case 3:
     ads1.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_2_3, true );
     break;
   default:
     DBPRINTLN( "ADS must be a number between 0 and 3!" );
     while( true );
     break;
  }

  while( millis() - time < 1000 ) {
    switch( adc ) {
      case 0:
        voltage = ads0.computeVolts( ads0.getLastConversionResults() );
        break;
      case 1:
        voltage = ads0.computeVolts( ads0.getLastConversionResults() );
        break;
      case 2:
        voltage = ads1.computeVolts( ads1.getLastConversionResults() );
        break;
      case 3:
        voltage = ads1.computeVolts( ads1.getLastConversionResults() );
        break;
      default:
        DBPRINTLN( "ADS must be a number between 0 and 3!" );
        while( true );
        break;
    }
    maxVolt = maxVolt > voltage ? maxVolt : voltage;
    minVolt = minVolt < voltage ? minVolt : voltage;
  }

  voltage = maxVolt < -minVolt ? - minVolt : maxVolt;
  
  double current = voltage / sqrt( 2.0 ) * _factor;

  switch( adc ) {
    case 0:
     ads0.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_0_1, false );
     break;
   case 1:
     ads0.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_2_3, false );
     break;
   case 2:
     ads1.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_0_1, false );
     break;
   case 3:
     ads1.startADCReading( ADS1X15_REG_CONFIG_MUX_DIFF_2_3, false );
     break;
   default:
     DBPRINTLN( "ADS must be a number between 0 and 3!" );
     while( true );
     break;
  }
  
  return( current );
  
}
#endif // CURRENT_H
