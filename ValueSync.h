#ifndef ValueSync_h
#define ValueSync_h

#define MESSAGE_START '#'
#define DELIMITER '|'

#define MESSAGE_START_STR "#"
#define DELIMITER_STR "|"

#define ALL_VALUES -1

#include "Arduino.h"

template<int numValues>
class ValueSender
{
  private:
  int* values[numValues];
  int prevValues[numValues];
  int valuesAdded;
  unsigned long lastValueSent;

  public:	
  ValueSender() : valuesAdded(0), lastValueSent(0)
  {
	
  }

  /**
   * Adds a new values to be synced between the two arduinos
   */
  ValueSender* addValue(int* value)
  {
	if(valuesAdded == numValues) return this;
	values[valuesAdded] = value;
	valuesAdded++;
	return this;
  }
  
  /**
   * Sends all values that have changed since the last call to syncValues
   */ 
  void syncValues()
  {
    int numValuesChanged = 0;
    for(int i = 0; i < valuesAdded; i++)
    {
      if(valueChanged(i))
        numValuesChanged++;
    }
    
    if(allValuesMinPackageSize() < numValuesChanged * singleValueMinPackageSize())
    {
      //Serial.print("We do one big package because "); Serial.println(numValuesChanged);
      sendAllValues();
      for(int i = 0; i < valuesAdded; i++)
        prevValues[i] = *values[i];
    }
    else
    {
      //Serial.print("We send single packages because "); Serial.println(numValuesChanged);
      for(int i = 0; i < valuesAdded; i++)
      {
        if(*values[i] != prevValues[i])
          sendValue(i);
        prevValues[i] = *values[i];
      }
    }
  }
  
  boolean valueChanged(int index)
  {
      return *values[index] != prevValues[index];
  }
  
  boolean valueChanged(int* value)
  {
    return valueChanged(indexForValue(value));
  }
  
  /**
   * Sends a value. You need to add it firtst via addValue.
   */
  boolean sendValue(int* value)
  {
    return sendValue(indexForValue(value));
  }
  
  int indexForValue(int* value)
  {
    for(int i = 0; i < valuesAdded; i++)
      if(values[i] == value)
        return i;
    return -1;
  }
  
  /**
   * Sends the x'st value you added to the sender.
   * Returns true if sending was successfull.
   */
  boolean sendValue(int index)
  {
    if(!(index < numValues && index >= 0))
      return false;
  
    Serial.print(MESSAGE_START);
    Serial.print(index);
    Serial.print(DELIMITER);
    Serial.print(*values[index]);
    Serial.print(DELIMITER);
    
    lastValueSent = millis();
    
    return true;
  }
  
  void sendKeepalive()
  {
    Serial.print(MESSAGE_START);
    lastValueSent = millis();
  }
  
  /**
   * Sends all values that have been added
   */
  void sendAllValues()
  {
    Serial.print(MESSAGE_START);
    Serial.print(ALL_VALUES);
    Serial.print(DELIMITER);
    for(int i = 0; i < valuesAdded; i++)
    {
      Serial.print(*values[i]);
      Serial.print(DELIMITER);
    }
    
    lastValueSent = millis();
  }
  
  inline int allValuesMinPackageSize()
  {
    return 2 + 2*valuesAdded;
  }
  
  inline int singleValueMinPackageSize()
  {
    return 5;
  }
  
  unsigned long timeSinceLastMessage()
  {
    return millis() - lastValueSent;
  }
};

template<int numValues>
class ValueReceiver
{
  private:
  int* values[numValues];
  int valuesAdded;
  unsigned long lastValueReceived;
  
  public:
  ValueReceiver() : valuesAdded(0), lastValueReceived(0)
  {

  }
  
   /**
   * Adds a new values to be synced between the two arduinos
   */
  ValueReceiver* addValue(int* value)
  {
	if(valuesAdded == numValues) return this;
	values[valuesAdded] = value;
	valuesAdded++;
	return this;
  }

   /**
   * Blocks until a new value can be read from the stream and sets the according variable.
   */
  void receiveValue()
  {
    if(!Serial.find(MESSAGE_START_STR)) return; //Read the header
    lastValueReceived = millis();

    int index = Serial.parseInt(); //Read the type
    
    if(index == ALL_VALUES)
    {
      if(Serial.read() != DELIMITER) return;
      for(int i = 0; i < valuesAdded; i++) 
      {
        *values[i] = Serial.parseInt();
        if(Serial.read() != DELIMITER) return;
      }
    }
    else
    {
      if(Serial.read() != DELIMITER) return;
      int value = Serial.parseInt();
      *values[index] = value;
    }
  }
  
  unsigned long timeSinceLastMessage()
  {
    return millis() - lastValueReceived;
  }
};

#endif
