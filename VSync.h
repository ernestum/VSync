/*
    VSync Library
    
    This is the VSync library for the arduino platform. It can 'magically' synchronize
    variables on your Arduino with variables on other Arduinos or in your Processing sketch.
    
    
    Use cases:
    
    For example if you read the heading off a digital compass and store it in a variable
    on your Arduino, you can sync that variable with a Processing sketch. In the Processing
    sketch you can use it to visualize the digital compass reading.
    Maybe you have many settings (color of a RGB LED; angle of a servo motor; parameters
    for a PID algorithm ...), that often need to be changed during the runtime of your Arduino.
    Simply synchronize the position of a sliders in your Processing sketch
    with the arduino to transfer transfer a parameter.
    
    
    Technical details:
    (boring, don't read if you just want to use the library)
    
    VSync uses Serial to synchronize int variables, which means that it will work through your
    USB cable as well as with your XBee. 
    It also means it can only do integers and will block any other (e.g. debugging) uses of
    the Serial interface.
    
    The library uses a very simple human readable protocol that is optimized for minimal 
    data traffic (e.g. only when a value changes it is sent over the line).
    The protocol is package based with two kinds of packages:
      * Full-Sync-Package: contains the values of all variables
      * Diff-Sync-Package: contains the values only of variables that changed
    The Full-Sync-Package looks like this: A<val1>|<val1>|<val3># if you are syncing three
    variables. E.g if the variables have the values 4, -8, 555 the package looks like:
      A|4|-8|555#
    The A at the beginning stands for all values, the # is the end-charakter.
    The Diff-Sync-Package looks like this: 0|4|2|555# if the first and the last value
    changed to 4 and 555. If only few (1 or 2) variables changed a Diff-Sync-Package is sent;
    if lots of variables changed it is more efficient to send a Full-Sync-Package, because
    the id's of the variables do not need to be sent.
    
    Author: Maximilian Ernestus
*/

#ifndef ValueSync_h
#define ValueSync_h

#include "Arduino.h"

#define VSYNC_MESSAGE_END '#'
#define VSYNC_DELIMITER '|'
#define VSYNC_ALL 'A'

template<int numVariables>
class ValueSender
{
private:
  int* variables[numVariables];
  int prevValues[numVariables];
  int variablesAdded;
  Stream* serial;
//   unsigned long lastValueSent;
  
  /**
   * Sends the x'st value you added to the sender.
   * Returns true if sending was successfull.
   */
  boolean sendValue(int index)
  {
    if(!(index < numVariables && index >= 0))
      return false;
  
    serial->print(index);
    serial->print(VSYNC_DELIMITER);
    serial->print(*variables[index]);
    
    return true;
  }
  
  boolean sendChangedValues()
  {
    boolean firstValueSent = false;
      for(int i = 0; i < variablesAdded; i++)
      {
        if(variableChanged(i))
	{
	  if(firstValueSent) 
	    serial->write(VSYNC_DELIMITER);
          sendValue(i);
	  firstValueSent = true;
	  
	}
      }
      serial->print(VSYNC_MESSAGE_END);
  }
  
  inline int allValuesMinPackageSize()
  {
    return 2 + 2*variablesAdded;
  }
  
  inline int singleValueMinPackageSize()
  {
    return 4;
  }
  
//   void sendKeepalive()
//   {
//     serial->print(VSYNC_MESSAGE_END);
//     lastValueSent = millis();
//   }

public:	
  ValueSender(Stream &serial = Serial) : variablesAdded(0), serial(&serial) //, lastValueSent(0)
  {
	
  }

  /**
   * Adds a new variables to be synced between the two arduinos
   */
  ValueSender* observe(int &variable)
  {
	if(variablesAdded == numVariables) return this;
	variables[variablesAdded] = &variable;
	variablesAdded++;
	return this;
  }
  
  /**
   * Sends all variables that have changed since the last call to syncValues
   */ 
  void sync()
  {
    int numVariablesChanged = 0;
    for(int i = 0; i < variablesAdded; i++)
      if(variableChanged(i))
        numVariablesChanged++;
    
    if(numVariablesChanged == 0)
      return; //TODO: maybe check for keepalives that need to be sent?
    
    if(allValuesMinPackageSize() < numVariablesChanged * singleValueMinPackageSize())
      sendAllVariables();
    else
      sendChangedValues();
    
    for(int i = 0; i < variablesAdded; i++)
      prevValues[i] = *variables[i];
      
//     lastValueSent = millis();
  }
  
  /**
   * Check if a variable has been changed since the last sync.
   */
  boolean variableChanged(int index)
  {
      return *variables[index] != prevValues[index];
  }
    
  /**
   * Sends all values that have been added
   */
  void sendAllVariables()
  {
    serial->print(VSYNC_ALL);
    for(int i = 0; i < variablesAdded; i++)
    {
      serial->print(VSYNC_DELIMITER);
      serial->print(*variables[i]);
    }
    serial->print(VSYNC_MESSAGE_END);
  }
  
//   unsigned long timeSinceLastMessage()
//   {
//     return millis() - lastValueSent;
//   }
};

template<int numVariables>
class ValueReceiver
{
private:
  int* variables[numVariables];
  int variablesAdded;
//   unsigned long lastValueReceived;
  String readbuffer;
  Stream* serial;
  
  void advanceCursor(int &cstart, int &cend, String &message)
  {
    cstart = cend+1;
    cend = message.indexOf(VSYNC_DELIMITER, cstart+1);   
  }
  
  void analyzeMessage(String message)
  {
    if(message[0] == VSYNC_ALL) //It is a Full-Sync-Package
    {
      message += VSYNC_DELIMITER;
      for(int cstart = 2, cend = message.indexOf(VSYNC_DELIMITER, 2), i = 0; cend > 0 && i < variablesAdded; advanceCursor(cstart, cend, message), i++)
      {
	*variables[i] = message.substring(cstart, cend).toInt();
      }
    }
    else //It is a Diff-Sync-Package
    {
      for(int cstart = 0, cend = message.indexOf(VSYNC_DELIMITER), i = 0; cend > 0 && i < variablesAdded; i++)
      {
	//Read the index
	int vIndex = message.substring(cstart, cend).toInt();
	advanceCursor(cstart, cend, message);
	
	//Read the value
	int value = message.substring(cstart, cend).toInt();
	advanceCursor(cstart, cend, message);
	
	//Update the variable with the index to the new value
	*variables[vIndex] = value;
      }
    }
  }
  
public:
  
  /**
   * Creates a new value receiver.
   */
  ValueReceiver(Stream &serial = Serial) : variablesAdded(0), readbuffer(""), serial(&serial) //, lastValueReceived(0)
  {

  }
  
   /**
   * Adds a new variables to be synced.
   */
  ValueReceiver* observe(int &variable)
  {
	if(variablesAdded == numVariables) return this; //check if we still can observe more variables
	variables[variablesAdded] = &variable; //store the pointer to the variable in an array
	variablesAdded++;
	return this;
  }

   /**
   * Synchronizes the observed variables.
   */
  void sync()
  {
    while(serial->available() > 0)
    {
      char in = serial->read();
//       serial->print(in);
      if(in == VSYNC_MESSAGE_END) //if the end of the message is reached
      {
	analyzeMessage(readbuffer); //analyze and update variables
	readbuffer = ""; //clear the reading buffer
      }
      else
      {
	readbuffer += in; //Fill the reading buffer
      }
    }
  }

  
//   unsigned long timeSinceLastMessage()
//   {
//     return millis() - lastValueReceived;
//   }
};

#endif
