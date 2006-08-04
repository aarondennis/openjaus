// File Name: createEventMessage.c
//
// Written By: Danny Kent (jaus AT dannykent DOT com), Tom Galluzzo (galluzzo AT gmail DOT com)
//
// Version: 3.2
//
// Date: 08/04/06
//
// Description: This file defines the functionality of a CreateEventMessage



#include <stdlib.h>
#include <string.h>
#include <cimar.h>
#include "cimar/jaus.h"

static const int commandCode = JAUS_CREATE_EVENT;
static const int maxDataSizeBytes = 512000; // Max Message size: 500K

static JausBoolean headerFromBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static JausBoolean headerToBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);

static JausBoolean dataFromBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static int dataToBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static void dataInitialize(CreateEventMessage message);
static void dataDestroy(CreateEventMessage message);

// ************************************************************************************************************** //
//                                    USER CONFIGURED FUNCTIONS
// ************************************************************************************************************** //

// Initializes the message-specific fields
static void dataInitialize(CreateEventMessage message)
{
	message->expFlag = JAUS_EXPERIMENTAL_MESSAGE;
	// Set initial values of message fields
	message->presenceVector = newJausBytePresenceVector();	// 1: Presence Vector
	message->messageCode = newJausUnsignedShort(0);			// 2: Command Code of the resulting query
	message->eventType = newJausByte(0);					// 3: Enumeration of Event types
	message->eventBoundary = newJausByte(0);				// 4: Enumeration of Event Boundary Conditions
	message->limitDataType = newJausByte(0);				// 5: Enumeration of data types for fields 7 & 8
	message->limitDataField = newJausByte(0);				// 6: Field from Report for Limit Trigger
	message->lowerLimit = jausEventLimitCreate();			// 7: Lower Event Limit
	message->upperLimit = jausEventLimitCreate();			// 8: Upper Event Limit
	message->stateLimit = jausEventLimitCreate();			// 9: State Event Limit used for Equal Boundary
	message->requestedMinimumRate = newJausDouble(0.0);		// 10: For Periodic Events for unchanging value, Scaled UnsignedShort (0, 1092)
	message->requestedUpdateRate = newJausDouble(0.0);		// 11: For Periodic Events, Scaled UnsignedShort (0, 1092)
	message->queryMessage = jausMessageCreate();			// 12: Query Message (including header) to use for response	
}

// Destructs the message-specific fields
static void dataDestroy(CreateEventMessage message)
{
	// Free message fields
	jausMessageDestroy(message->queryMessage);
	if(message->lowerLimit) jausEventLimitDestroy(message->lowerLimit);
	if(message->upperLimit) jausEventLimitDestroy(message->upperLimit);
	if(message->stateLimit) jausEventLimitDestroy(message->stateLimit);
}

// Return boolean of success
static JausBoolean dataFromBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	int index = 0;
	JausUnsignedShort tempUShort;
	
	if(bufferSizeBytes == message->dataSize)
	{
		// Unpack Message Fields from Buffer
		// Presence Vector
		if(!jausBytePresenceVectorFromBuffer(&message->presenceVector, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_BYTE_PRESENCE_VECTOR_SIZE_BYTES;

		// Message Code
		if(!jausUnsignedShortFromBuffer(&message->messageCode, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;		

		// Event Type
		if(!jausByteFromBuffer(&message->eventType, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_BYTE_SIZE_BYTES;
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_BOUNDARY_BIT))
		{
			// Event Boundary
			if(!jausByteFromBuffer(&message->eventBoundary, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
		}

		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_DATA_TYPE_BIT))
		{
			// Data Type
			if(!jausByteFromBuffer(&message->limitDataType, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_DATA_FIELD_BIT))
		{
			// Data Field
			if(!jausByteFromBuffer(&message->limitDataField, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_LOWER_LIMIT_BIT))
		{		
			// Lower Limit
			message->lowerLimit = jausEventLimitFromBuffer(buffer+index, bufferSizeBytes-index, message->limitDataType);
			if(!message->lowerLimit) return JAUS_FALSE;
			index += jausEventLimitByteSize(message->limitDataType);
		}
		else
		{
			message->lowerLimit = NULL;
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_UPPER_LIMIT_BIT))
		{		
			// Upper Limit
			message->upperLimit = jausEventLimitFromBuffer(buffer+index, bufferSizeBytes-index, message->limitDataType);
			if(!message->upperLimit) return JAUS_FALSE;
			index += jausEventLimitByteSize(message->limitDataType);
		}
		else
		{
			message->upperLimit = NULL;
		}

		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_STATE_LIMIT_BIT))
		{		
			// State Limit
			message->stateLimit = jausEventLimitFromBuffer(buffer+index, bufferSizeBytes-index, message->limitDataType);
			if(!message->stateLimit) return JAUS_FALSE;
			index += jausEventLimitByteSize(message->limitDataType);
		}
		else
		{
			message->stateLimit = NULL;
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_MINIMUM_RATE_BIT))
		{		
			// Minimum Periodic Rate 
			if(!jausUnsignedShortFromBuffer(&tempUShort, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
			
			// Scaled Int (0, 1092);
			message->requestedMinimumRate = jausUnsignedShortToDouble(tempUShort, 0, 1092);
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_REQUESTED_RATE_BIT))
		{		
			// Periodic Rate 
			if(!jausUnsignedShortFromBuffer(&tempUShort, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
			
			// Scaled Int (0, 1092);
			message->requestedUpdateRate = jausUnsignedShortToDouble(tempUShort, 0, 1092);
		}
		
		message->queryMessage = jausMessageCreate();
		
		// Jaus Message
		if(!jausMessageFromBuffer(message->queryMessage, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += jausMessageSize(message->queryMessage);
		
		return JAUS_TRUE;
	}
	else
	{
		return JAUS_FALSE;
	}
}

// Returns number of bytes put into the buffer
static int dataToBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	int index = 0;
	JausUnsignedShort tempUShort;

	if(bufferSizeBytes >= message->dataSize)
	{
		// Unpack Message Fields from Buffer
		// Presence Vector
		if(!jausBytePresenceVectorToBuffer(message->presenceVector, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_BYTE_PRESENCE_VECTOR_SIZE_BYTES;
		
		// Message Code
		if(!jausUnsignedShortToBuffer(message->messageCode, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
		
		// Event Type
		if(!jausByteToBuffer(message->eventType, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_BYTE_SIZE_BYTES;
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_BOUNDARY_BIT))
		{
			// Event Boundary
			if(!jausByteToBuffer(message->eventBoundary, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
		}

		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_DATA_TYPE_BIT))
		{
			// Data Type
			if(!jausByteToBuffer(message->limitDataType, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_DATA_FIELD_BIT))
		{
			// Data Field
			if(!jausByteToBuffer(message->limitDataField, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_LOWER_LIMIT_BIT))
		{		
			// Lower Limit
			if(!jausEventLimitToBuffer(message->lowerLimit, buffer+index, bufferSizeBytes-index, message->limitDataType)) return JAUS_FALSE;
			index += jausEventLimitByteSize(message->limitDataType);
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_UPPER_LIMIT_BIT))
		{		
			// Upper Limit
			if(!jausEventLimitToBuffer(message->upperLimit, buffer+index, bufferSizeBytes-index, message->limitDataType)) return JAUS_FALSE;
			index += jausEventLimitByteSize(message->limitDataType);
		}

		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_STATE_LIMIT_BIT))
		{		
			// State Limit
			if(!jausEventLimitToBuffer(message->stateLimit, buffer+index, bufferSizeBytes-index, message->limitDataType)) return JAUS_FALSE;
			index += jausEventLimitByteSize(message->limitDataType);
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_MINIMUM_RATE_BIT))
		{		
			// Scaled Int (0, 1092);
			tempUShort = jausUnsignedShortFromDouble(message->requestedMinimumRate, 0, 1092);

			// Minimum Periodic Rate 
			if(!jausUnsignedShortToBuffer(tempUShort, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
		}
		
		if(jausBytePresenceVectorIsBitSet(message->presenceVector, CREATE_EVENT_PV_REQUESTED_RATE_BIT))
		{		
			// Scaled Int (0, 1092);
			tempUShort = jausUnsignedShortFromDouble(message->requestedUpdateRate, 0, 1092);

			// Minimum Periodic Rate 
			if(!jausUnsignedShortToBuffer(tempUShort, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
		}
		
		// Jaus Message
		if(!jausMessageToBuffer(message->queryMessage, buffer+index, bufferSizeBytes)) return JAUS_FALSE;
		index += jausMessageSize(message->queryMessage);
	}

	return index;
}

// ************************************************************************************************************** //
//                                    NON-USER CONFIGURED FUNCTIONS
// ************************************************************************************************************** //

CreateEventMessage createEventMessageCreate(void)
{
	CreateEventMessage message;

	message = (CreateEventMessage)malloc( sizeof(CreateEventMessageStruct) );
	if(message == NULL)
	{
		return NULL;
	}
	
	// Initialize Values
	message->priority = JAUS_DEFAULT_PRIORITY;
	message->ackNak = JAUS_ACK_NAK_NOT_REQUIRED;
	message->scFlag = JAUS_NOT_SERVICE_CONNECTION_MESSAGE;
	message->expFlag = JAUS_NOT_EXPERIMENTAL_MESSAGE;
	message->version = JAUS_VERSION_3_2;
	message->reserved = 0;
	message->commandCode = commandCode;
	message->destination = jausAddressCreate();
	message->source = jausAddressCreate();
	message->dataFlag = JAUS_SINGLE_DATA_PACKET;
	message->dataSize = maxDataSizeBytes;
	message->sequenceNumber = 0;
	
	dataInitialize(message);
	
	return message;	
}

void createEventMessageDestroy(CreateEventMessage message)
{
	dataDestroy(message);
	jausAddressDestroy(message->source);
	jausAddressDestroy(message->destination);
	free(message);
	message = NULL;
}

JausBoolean createEventMessageFromBuffer(CreateEventMessage message, unsigned char* buffer, unsigned int bufferSizeBytes)
{
	int index = 0;
	
	if(!strncmp((char *)buffer, JAUS_UDP_HEADER, JAUS_UDP_HEADER_SIZE_BYTES)) // equals 1 if same
	{
		index += JAUS_UDP_HEADER_SIZE_BYTES;
	}

	if(headerFromBuffer(message, buffer+index, bufferSizeBytes-index))
	{
		index += JAUS_HEADER_SIZE_BYTES;
		if(dataFromBuffer(message, buffer+index, bufferSizeBytes-index))
		{
			return JAUS_TRUE;
		}
		else
		{
			return JAUS_FALSE;
		}
	}
	else
	{
		return JAUS_FALSE;
	}
}

JausBoolean createEventMessageToBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	if(bufferSizeBytes < createEventMessageSize(message))
	{
		return JAUS_FALSE; //improper size	
	}
	else
	{	
		message->dataSize = dataToBuffer(message, buffer+JAUS_HEADER_SIZE_BYTES, bufferSizeBytes - JAUS_HEADER_SIZE_BYTES);
		if(headerToBuffer(message, buffer, bufferSizeBytes))
		{
			return JAUS_TRUE;
		}
		else
		{
			return JAUS_FALSE; // headerToCreateEventBuffer failed
		}
	}
}

JausBoolean createEventMessageToUdpBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	if(bufferSizeBytes < createEventMessageUdpSize(message))
	{
		return JAUS_FALSE; // improper size
	}
	else
	{
		strncpy( (char *) buffer, JAUS_UDP_HEADER, JAUS_UDP_HEADER_SIZE_BYTES); //copies the UDP header into the buffer
		return createEventMessageToBuffer(message, buffer+JAUS_UDP_HEADER_SIZE_BYTES, bufferSizeBytes - JAUS_UDP_HEADER_SIZE_BYTES);
	}
}

CreateEventMessage createEventMessageFromJausMessage(JausMessage jausMessage)
{
	CreateEventMessage message;

	if(jausMessage->commandCode != commandCode)
	{
		return NULL; // Wrong message type
	}
	else
	{
		message = (CreateEventMessage)malloc( sizeof(CreateEventMessageStruct) );
		if(message == NULL)
		{
			return NULL;
		}
		
		message->properties = jausMessage->properties;
		message->commandCode = jausMessage->commandCode;
		message->destination = jausAddressCreate();
		message->destination->id = jausMessage->destination->id;
		message->source = jausAddressCreate();
		message->source->id = jausMessage->source->id;
		message->dataControl = jausMessage->dataControl;
		message->sequenceNumber = jausMessage->sequenceNumber;
		
		// Unpack jausMessage->data
		if(dataFromBuffer(message, jausMessage->data, jausMessage->dataSize))
		{
			return message;
		}
		else
		{
			return NULL;
		}
	}
}

JausMessage createEventMessageToJausMessage(CreateEventMessage message)
{
	JausMessage jausMessage;
	
	jausMessage = (JausMessage)malloc( sizeof(JausMessageStruct) );
	if(jausMessage == NULL)
	{
		return NULL;
	}	
	
	jausMessage->properties = message->properties;
	jausMessage->commandCode = message->commandCode;
	jausMessage->destination = jausAddressCreate();
	jausMessage->destination->id = message->destination->id;
	jausMessage->source = jausAddressCreate();
	jausMessage->source->id = message->source->id;
	jausMessage->dataControl = message->dataControl;
	jausMessage->sequenceNumber = message->sequenceNumber;
	
	jausMessage->data = (unsigned char *)malloc(message->dataSize);
	jausMessage->dataSize = dataToBuffer(message, jausMessage->data, message->dataSize);
	
	return jausMessage;
}

unsigned int createEventMessageUdpSize(CreateEventMessage message)
{
	return (unsigned int)(message->dataSize + JAUS_HEADER_SIZE_BYTES + JAUS_UDP_HEADER_SIZE_BYTES);
}

unsigned int createEventMessageSize(CreateEventMessage message)
{
	return (unsigned int)(message->dataSize + JAUS_HEADER_SIZE_BYTES);
}

//********************* PRIVATE HEADER FUNCTIONS **********************//

static JausBoolean headerFromBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	if(bufferSizeBytes < JAUS_HEADER_SIZE_BYTES)
	{ 
		return JAUS_FALSE;
	}
	else
	{
		// unpack header
		message->properties = buffer[0] + (buffer[1] << 8);
		message->commandCode = buffer[2] + (buffer[3] << 8);
	
		message->destination->instance = buffer[4];
		message->destination->component = buffer[5];
		message->destination->node = buffer[6];
		message->destination->subsystem = buffer[7];
	
		message->source->instance = buffer[8];
		message->source->component = buffer[9];
		message->source->node = buffer[10];
		message->source->subsystem = buffer[11];
		
		message->dataControl = buffer[12] + (buffer[13] << 8);
		message->sequenceNumber = buffer[14] + (buffer[15] << 8);
		
		return JAUS_TRUE;;
	}
}

static JausBoolean headerToBuffer(CreateEventMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	if(bufferSizeBytes < JAUS_HEADER_SIZE_BYTES)
	{
		return JAUS_FALSE;
	}
	else
	{	
		buffer[0] = (unsigned char)(message->properties & 0xFF);
		buffer[1] = (unsigned char)((message->properties & 0xFF00) >> 8);

		buffer[2] = (unsigned char)(message->commandCode & 0xFF);
		buffer[3] = (unsigned char)((message->commandCode & 0xFF00) >> 8);

		buffer[4] = (unsigned char)(message->destination->instance & 0xFF);
		buffer[5] = (unsigned char)(message->destination->component & 0xFF);
		buffer[6] = (unsigned char)(message->destination->node & 0xFF);
		buffer[7] = (unsigned char)(message->destination->subsystem & 0xFF);

		buffer[8] = (unsigned char)(message->source->instance & 0xFF);
		buffer[9] = (unsigned char)(message->source->component & 0xFF);
		buffer[10] = (unsigned char)(message->source->node & 0xFF);
		buffer[11] = (unsigned char)(message->source->subsystem & 0xFF);
		
		buffer[12] = (unsigned char)(message->dataControl & 0xFF);
		buffer[13] = (unsigned char)((message->dataControl & 0xFF00) >> 8);

		buffer[14] = (unsigned char)(message->sequenceNumber & 0xFF);
		buffer[15] = (unsigned char)((message->sequenceNumber & 0xFF00) >> 8);
		
		return JAUS_TRUE;
	}
}

