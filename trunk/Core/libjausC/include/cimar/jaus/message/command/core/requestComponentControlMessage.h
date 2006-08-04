// File Name: requestComponentControlMessage.h
//
// Written By: Danny Kent (jaus AT dannykent DOT com), Tom Galluzzo 
//
// Version: 3.2
//
// Date: 08/04/06
//
// Description: This file defines the attributes of a RequestComponentControlMessage

#ifndef REQUEST_COMPONENT_CONTROL_MESSAGE_H
#define REQUEST_COMPONENT_CONTROL_MESSAGE_H

#include "cimar/jaus.h"

typedef struct
{
	// Include all parameters from a JausMessage structure:
	// Header Properties
	union
	{
		struct
		{
			// Properties by bit fields
			#ifdef JAUS_BIG_ENDIAN
				JausUnsignedShort reserved:2;
				JausUnsignedShort version:6;
				JausUnsignedShort expFlag:1;
				JausUnsignedShort scFlag:1;
				JausUnsignedShort ackNak:2;
				JausUnsignedShort priority:4; 
			#elif JAUS_LITTLE_ENDIAN
				JausUnsignedShort priority:4; 
				JausUnsignedShort ackNak:2;
				JausUnsignedShort scFlag:1; 
				JausUnsignedShort expFlag:1;
				JausUnsignedShort version:6; 
				JausUnsignedShort reserved:2;
			#else
				#error "Please define system endianess (see jaus.h)"
			#endif
		};
		JausUnsignedShort properties;		
	};

	JausUnsignedShort commandCode; 

	JausAddress destination;

	JausAddress source;

	union
	{
		struct
		{
			JausUnsignedInteger dataSize;
			JausUnsignedInteger dataFlag;
		};
		JausUnsignedLong dataControl;		
	};
	
	JausUnsignedShort sequenceNumber;

	JausByte authorityCode;
	
}RequestComponentControlMessageStruct;

typedef RequestComponentControlMessageStruct* RequestComponentControlMessage;

RequestComponentControlMessage requestComponentControlMessageCreate(void);
void requestComponentControlMessageDestroy(RequestComponentControlMessage);

JausBoolean requestComponentControlMessageFromBuffer(RequestComponentControlMessage message, unsigned char* buffer, unsigned int bufferSizeBytes);
JausBoolean requestComponentControlMessageToBuffer(RequestComponentControlMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
JausBoolean requestComponentControlMessageToUdpBuffer(RequestComponentControlMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);

RequestComponentControlMessage requestComponentControlMessageFromJausMessage(JausMessage jausMessage);
JausMessage requestComponentControlMessageToJausMessage(RequestComponentControlMessage message);

unsigned int requestComponentControlMessageUdpSize(RequestComponentControlMessage message);
unsigned int requestComponentControlMessageSize(RequestComponentControlMessage message);


#endif // REQUEST_COMPONENT_CONTROL_MESSAGE_H
