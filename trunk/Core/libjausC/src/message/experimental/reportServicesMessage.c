// File Name: reportServicesMessage.c
//
// Written By: Bob Toucthon, inspired by Danny Kent
//
// Version: 3.2
//
// Date: 08/04/06
//
// This file defines the functionality of a ReportServicesMessage

#include <stdlib.h>
#include <string.h>
#include "cimar/jaus.h"

static const int commandCode = JAUS_REPORT_SERVICES;
static const int maxDataSizeBytes = 512000;

static JausBoolean headerFromBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static JausBoolean headerToBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);

static JausBoolean dataFromBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static int dataToBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes);
static void dataInitialize(ReportServicesMessage message);
static void dataDestroy(ReportServicesMessage message);

// ************************************************************************************************************** //
//                                    USER CONFIGURED FUNCTIONS
// ************************************************************************************************************** //

// Initializes the message-specific fields
static void dataInitialize(ReportServicesMessage message)
{
	// Set Experimental Flag
	message->expFlag = JAUS_EXPERIMENTAL_MESSAGE;

	// Set initial values of message fields
	message->jausServices = vectorCreate();
}

// Destructs the message-specific fields
static void dataDestroy(ReportServicesMessage message)
{
	// Free message fields
	vectorDestroy(message->jausServices, (void *)jausServiceDestroy);
}

// Return boolean of success
static JausBoolean dataFromBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	int index = 0, i, j;
	JausByte serviceCount = 0;
	JausByte messageCount = 0;
	JausService tempService;
	JausCommand tempCommand;
	JausUnsignedShort tempUShort;
	
	if(bufferSizeBytes == message->dataSize)
	{
		// Unpack Message Fields from Buffer
		// # services
		if(!jausByteFromBuffer(&serviceCount, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_BYTE_SIZE_BYTES;
		
		if(serviceCount < 1) return JAUS_FALSE;
		message->jausServices = vectorCreate();
		
		for(i = 0; i < serviceCount; i++)
		{			
			// service type
			if(!jausUnsignedShortFromBuffer(&tempUShort, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
		
			//Create service
			tempService = jausServiceCreate(tempUShort);
		
			//read number of input messages for this service
			if(!jausByteFromBuffer(&messageCount, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
			
			for(j = 0; j < messageCount; j++)
			{
				// create command
				tempCommand = jausCommandCreate();
				
				//read command code
				if(!jausUnsignedShortFromBuffer(&tempCommand->commandCode, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;

				//read presence vector
				if(!jausUnsignedIntegerFromBuffer(&tempCommand->presenceVector, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_UNSIGNED_INTEGER_SIZE_BYTES;

				// add command to service
				vectorAdd(tempService->inputJausCommands, tempCommand);
			}
			
			//read number of output messages for this service
			if(!jausByteFromBuffer(&messageCount, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
			
			for(j = 0; j < messageCount; j++)
			{
				// create command
				tempCommand = jausCommandCreate();
				//read command code
				if(!jausUnsignedShortFromBuffer(&tempCommand->commandCode, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;

				//read presence vector
				if(!jausUnsignedIntegerFromBuffer(&tempCommand->presenceVector, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_UNSIGNED_INTEGER_SIZE_BYTES;

				// add command to service
				vectorAdd(tempService->outputJausCommands, tempCommand);
			}

			// Add this node to the message object
			vectorAdd(message->jausServices, tempService);
		}
		return JAUS_TRUE;
	}
	else
	{
		return JAUS_FALSE;
	}
}

// Returns number of bytes put into the buffer
static int dataToBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	int index = 0;
	int i = 0;
	int j = 0;
	JausService service;
	JausCommand command;

	if(bufferSizeBytes >= message->dataSize)
	{
		// Pack Message Fields to Buffer
	    // # Services
		if(!jausByteToBuffer(message->jausServices->elementCount, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_BYTE_SIZE_BYTES;
    
		// Loop through all services
	    for(i = 0; i < message->jausServices->elementCount; i++)
		{
			service = (JausService) message->jausServices->elementData[i];
			
			// Service Type
			if(!jausUnsignedShortToBuffer(service->type, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
			
			// Number of Input Commands
			if(!jausByteToBuffer(service->inputJausCommands->elementCount, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
			
			// Loop through inputs
			for(j = 0; j < service->inputJausCommands->elementCount; j++)
			{
				command = (JausCommand)service->inputJausCommands->elementData[j];
				
				// Command Code
				if(!jausUnsignedShortToBuffer(command->commandCode, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
				
				// Presence Vector
				if(!jausUnsignedIntegerToBuffer(command->presenceVector, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_UNSIGNED_INTEGER_SIZE_BYTES;
			}

			// Number of Output Commands
			if(!jausByteToBuffer(service->outputJausCommands->elementCount, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
			index += JAUS_BYTE_SIZE_BYTES;
			
			// Loop through outputs
			for(j = 0; j < service->outputJausCommands->elementCount; j++)
			{
				command = (JausCommand)service->outputJausCommands->elementData[j];
				
				// Command Code
				if(!jausUnsignedShortToBuffer(command->commandCode, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
				
				// Presence Vector
				if(!jausUnsignedIntegerToBuffer(command->presenceVector, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_UNSIGNED_INTEGER_SIZE_BYTES;
			}
		}
	}
	return index;
}

// ************************************************************************************************************** //
//                                    NON-USER CONFIGURED FUNCTIONS
// ************************************************************************************************************** //

ReportServicesMessage reportServicesMessageCreate(void)
{
	ReportServicesMessage message;

	message = (ReportServicesMessage)malloc( sizeof(ReportServicesMessageStruct) );
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

void reportServicesMessageDestroy(ReportServicesMessage message)
{
	dataDestroy(message);
	jausAddressDestroy(message->source);
	jausAddressDestroy(message->destination);
	free(message);
}

JausBoolean reportServicesMessageFromBuffer(ReportServicesMessage message, unsigned char* buffer, unsigned int bufferSizeBytes)
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

JausBoolean reportServicesMessageToBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	if(bufferSizeBytes < reportServicesMessageSize(message))
	{
		return JAUS_FALSE; //improper size	
	}
	else
	{	
		if(headerToBuffer(message, buffer, bufferSizeBytes))
		{
			message->dataSize = dataToBuffer(message, buffer+JAUS_HEADER_SIZE_BYTES, bufferSizeBytes - JAUS_HEADER_SIZE_BYTES);
			return JAUS_TRUE;
		}
		else
		{
			return JAUS_FALSE; // headerToReportServicesBuffer failed
		}
	}
}

JausBoolean reportServicesMessageToUdpBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	if(bufferSizeBytes < reportServicesMessageUdpSize(message))
	{
		return JAUS_FALSE; // improper size
	}
	else
	{
		strncpy((char *)buffer, JAUS_UDP_HEADER, JAUS_UDP_HEADER_SIZE_BYTES); //copies the UDP header into the buffer
		return reportServicesMessageToBuffer(message, buffer+JAUS_UDP_HEADER_SIZE_BYTES, bufferSizeBytes - JAUS_UDP_HEADER_SIZE_BYTES);
	}
}

ReportServicesMessage reportServicesMessageFromJausMessage(JausMessage jausMessage)
{
	ReportServicesMessage message;
	
	if(jausMessage->commandCode != commandCode)
	{
		return NULL; // Wrong message type
	}
	else
	{
		message = (ReportServicesMessage)malloc( sizeof(ReportServicesMessageStruct) );
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

JausMessage reportServicesMessageToJausMessage(ReportServicesMessage message)
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

unsigned int reportServicesMessageUdpSize(ReportServicesMessage message)
{
	return (unsigned int)(message->dataSize + JAUS_HEADER_SIZE_BYTES + JAUS_UDP_HEADER_SIZE_BYTES);
}

unsigned int reportServicesMessageSize(ReportServicesMessage message)
{
	return (unsigned int)(message->dataSize + JAUS_HEADER_SIZE_BYTES);
}

//********************* PRIVATE HEADER FUNCTIONS **********************//

static JausBoolean headerFromBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
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
		
		return JAUS_TRUE;
	}
}

static JausBoolean headerToBuffer(ReportServicesMessage message, unsigned char *buffer, unsigned int bufferSizeBytes)
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

