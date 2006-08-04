// File Name: jausWorldModelFeatureClass.c
//
// Written By: Danny Kent (jaus AT dannykent DOT com), Tom Galluzzo (galluzzo AT gmail DOT com)
//
// Version: 3.2
//
// Date: 08/04/06
//
// Description:

#include <string.h>
#include <stdlib.h>

#include <cimar.h>
#include "cimar/jaus.h"

// JausWorldModelFeatureClass Constructor
JausWorldModelFeatureClass featureClassCreate(void)
{
	JausWorldModelFeatureClass fcClass;
	fcClass = (JausWorldModelFeatureClass) malloc(sizeof(JausWorldModelFeatureClassStruct));
	if(fcClass)
	{
		fcClass->id = newJausUnsignedShort(0);							// Enumeration, defined by system
		memset(fcClass->metaData, 0, JAUS_WM_FC_METADATA_STRING_LENGTH);	// String of Metadata information defined for this Feature Class
		fcClass->attribute = featureClassAttributeCreate();				// Feature Class Attribute data type and value
		return fcClass;
	}
	else
	{
		return NULL;
	}
}

// JausWorldModelFeatureClass Constructor (from Buffer)
JausWorldModelFeatureClass featureClassFromBuffer(unsigned char *buffer, unsigned int bufferSizeBytes)
{
	unsigned int index = 0;
	JausWorldModelFeatureClass fcClass;
	
	fcClass = (JausWorldModelFeatureClass) malloc(sizeof(JausWorldModelFeatureClassStruct));
	if(fcClass)
	{
		if(!jausUnsignedShortFromBuffer(&fcClass->id, buffer+index, bufferSizeBytes-index))
		{
			free(fcClass);
			return NULL;
		}
		index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
	
		fcClass->attribute = featureClassAttributeFromBuffer(buffer+index, bufferSizeBytes-index);
		if(!fcClass->attribute)
		{
			free(fcClass);
			return NULL;
		}

		return fcClass;
	}
	else
	{
		return NULL;
	}
}

// JausWorldModelFeatureClass To Buffer
JausBoolean featureClassToBuffer(JausWorldModelFeatureClass fcClass, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	unsigned int index = 0;
	
	if(fcClass)
	{
		// FC Id
		if(!jausUnsignedShortToBuffer(fcClass->id, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
		
		// FC Attribute Data Type & Value
		if(!featureClassAttributeToBuffer(fcClass->attribute, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		return JAUS_TRUE;
	}
	return JAUS_FALSE;
}

// JausWorldModelFeatureClass Destructor
void featureClassDestroy(JausWorldModelFeatureClass fcClass)
{
	if(fcClass)
	{
		featureClassAttributeDestroy(fcClass->attribute);
		free(fcClass);
		fcClass = NULL;
	}
}

// JausWorldModelFeatureClass Buffer Size
unsigned int featureClassSizeBytes(JausWorldModelFeatureClass fcClass)
{
	unsigned int size = 0;
	
	if(fcClass)
	{
		size += JAUS_UNSIGNED_SHORT_SIZE_BYTES;	// Class Id
		size += featureClassAttributeSizeBytes(fcClass->attribute);	// Size of attribute & data type
	}
	return size;
}

// JausWorldModelFeatureClass Copy
JausWorldModelFeatureClass featureClassCopy(JausWorldModelFeatureClass fcClass)
{
	JausWorldModelFeatureClass classCopy;
	
	classCopy = (JausWorldModelFeatureClass) malloc(sizeof(JausWorldModelFeatureClassStruct));
	if(classCopy)
	{
		memcpy(classCopy, fcClass, sizeof(JausWorldModelFeatureClassStruct));
		classCopy->attribute = featureClassAttributeCopy(fcClass->attribute);
		if(!classCopy->attribute)
		{
			free(classCopy);
			return NULL;
		}
		return classCopy;
	}
	else
	{
		return NULL;
	}
}

// JausWorldModelFeatureClassAttribute Constructor
JausWorldModelFeatureClassAttribute featureClassAttributeCreate(void)
{
	JausWorldModelFeatureClassAttribute attribute;
	attribute = (JausWorldModelFeatureClassAttribute) malloc(sizeof(JausWorldModelFeatureClassAttributeStruct));
	if(attribute)
	{
		memset(attribute, 0, sizeof(JausWorldModelFeatureClassAttributeStruct));
		attribute->dataType = newJausByte(JAUS_WM_OBJECT_DEFAULT_DATA);
		return attribute;
	}
	else
	{
		return NULL;
	}
}

// JausWorldModelFeatureClassAttribute Destructor
void featureClassAttributeDestroy(JausWorldModelFeatureClassAttribute attribute)
{
	if(attribute)
	{
		free(attribute);
		attribute = NULL;
	}
}

// JausWorldModelFeatureClassAttribute Constructor (from Buffer)
JausWorldModelFeatureClassAttribute featureClassAttributeFromBuffer(unsigned char *buffer, unsigned int bufferSizeBytes)
{
	unsigned int index = 0;
	JausWorldModelFeatureClassAttribute attribute;

	attribute = (JausWorldModelFeatureClassAttribute) malloc(sizeof(JausWorldModelFeatureClassAttributeStruct));
	if(attribute)
	{
		// Data Type
		if(!jausByteFromBuffer(&attribute->dataType, buffer+index, bufferSizeBytes-index))
		{
			free(attribute);
			return NULL;
		}
		index += JAUS_BYTE_SIZE_BYTES;
		
		switch(attribute->dataType)
		{
			case JAUS_WM_OBJECT_BYTE_DATA:
				if(!jausByteFromBuffer(&attribute->byteValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;
				
			case JAUS_WM_OBJECT_SHORT_DATA:
				if(!jausShortFromBuffer(&attribute->shortValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;

			case JAUS_WM_OBJECT_INTEGER_DATA:
				if(!jausIntegerFromBuffer(&attribute->integerValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;

			case JAUS_WM_OBJECT_LONG_DATA:
				if(!jausLongFromBuffer(&attribute->longValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;

			case JAUS_WM_OBJECT_UNSIGNED_SHORT_DATA:
				if(!jausUnsignedShortFromBuffer(&attribute->unsignedShortValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;

			case JAUS_WM_OBJECT_UNSIGNED_INTEGER_DATA:
				if(!jausUnsignedIntegerFromBuffer(&attribute->unsignedIntegerValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;

			case JAUS_WM_OBJECT_UNSIGNED_LONG_DATA:
				if(!jausUnsignedLongFromBuffer(&attribute->unsignedLongValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;

			case JAUS_WM_OBJECT_FLOAT_DATA:
				if(!jausFloatFromBuffer(&attribute->floatValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;

			case JAUS_WM_OBJECT_DOUBLE_DATA:
				if(!jausDoubleFromBuffer(&attribute->doubleValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				return attribute;

				break;

			case JAUS_WM_OBJECT_RGB_DATA:
				if(!jausByteFromBuffer(&attribute->redValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				index += JAUS_BYTE_SIZE_BYTES;

				if(!jausByteFromBuffer(&attribute->greenValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				index += JAUS_BYTE_SIZE_BYTES;

				if(!jausByteFromBuffer(&attribute->blueValue, buffer+index, bufferSizeBytes-index))
				{
					free(attribute);
					return NULL;
				}
				index += JAUS_BYTE_SIZE_BYTES;

				return attribute;

			default:
				free(attribute);
				return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

// JausWorldModelFeatureClassAttribute To Buffer
JausBoolean featureClassAttributeToBuffer(JausWorldModelFeatureClassAttribute attribute, unsigned char *buffer, unsigned int bufferSizeBytes)
{
	unsigned int index = 0;
	if(attribute)
	{
		// Data Type
		if(!jausByteToBuffer(attribute->dataType, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
		index += JAUS_BYTE_SIZE_BYTES;
		
		// Value
		switch(attribute->dataType)
		{
			case JAUS_WM_OBJECT_BYTE_DATA:
				if(!jausByteToBuffer(attribute->byteValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;
								
			case JAUS_WM_OBJECT_SHORT_DATA:
				if(!jausShortToBuffer(attribute->shortValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			case JAUS_WM_OBJECT_INTEGER_DATA:
				if(!jausIntegerToBuffer(attribute->integerValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			case JAUS_WM_OBJECT_LONG_DATA:
				if(!jausLongToBuffer(attribute->longValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			case JAUS_WM_OBJECT_UNSIGNED_SHORT_DATA:
				if(!jausUnsignedShortToBuffer(attribute->unsignedShortValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			case JAUS_WM_OBJECT_UNSIGNED_INTEGER_DATA:
				if(!jausUnsignedIntegerToBuffer(attribute->unsignedIntegerValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			case JAUS_WM_OBJECT_UNSIGNED_LONG_DATA:
				if(!jausUnsignedLongToBuffer(attribute->unsignedLongValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			case JAUS_WM_OBJECT_FLOAT_DATA:
				if(!jausFloatToBuffer(attribute->floatValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			case JAUS_WM_OBJECT_DOUBLE_DATA:
				if(!jausDoubleToBuffer(attribute->doubleValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			case JAUS_WM_OBJECT_RGB_DATA:
				if(!jausByteToBuffer(attribute->redValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_BYTE_SIZE_BYTES;

				if(!jausByteToBuffer(attribute->greenValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				index += JAUS_BYTE_SIZE_BYTES;

				if(!jausByteToBuffer(attribute->blueValue, buffer+index, bufferSizeBytes-index)) return JAUS_FALSE;
				return JAUS_TRUE;

			default:
				return JAUS_TRUE;
		}
	}
	return JAUS_FALSE;
}

// JausWorldModelFeatureClassAttribute Buffer Size
unsigned int featureClassAttributeSizeBytes(JausWorldModelFeatureClassAttribute attribute)
{
	unsigned int size = 0;
	
	if(attribute)
	{
		size += JAUS_BYTE_SIZE_BYTES;
		switch(attribute->dataType)
		{
			case JAUS_WM_OBJECT_BYTE_DATA:
				size += JAUS_BYTE_SIZE_BYTES;
				return size;
				break;
				
			case JAUS_WM_OBJECT_SHORT_DATA:
				size += JAUS_SHORT_SIZE_BYTES;
				return size;
				break;

			case JAUS_WM_OBJECT_INTEGER_DATA:
				size += JAUS_INTEGER_SIZE_BYTES;
				return size;
				break;

			case JAUS_WM_OBJECT_LONG_DATA:
				size += JAUS_LONG_SIZE_BYTES;
				return size;
				break;

			case JAUS_WM_OBJECT_UNSIGNED_SHORT_DATA:
				size += JAUS_UNSIGNED_SHORT_SIZE_BYTES;
				return size;
				break;

			case JAUS_WM_OBJECT_UNSIGNED_INTEGER_DATA:
				size += JAUS_UNSIGNED_INTEGER_SIZE_BYTES;
				return size;
				break;

			case JAUS_WM_OBJECT_UNSIGNED_LONG_DATA:
				size += JAUS_UNSIGNED_LONG_SIZE_BYTES;
				return size;
				break;

			case JAUS_WM_OBJECT_FLOAT_DATA:
				size += JAUS_FLOAT_SIZE_BYTES;
				return size;
				break;

			case JAUS_WM_OBJECT_DOUBLE_DATA:
				size += JAUS_DOUBLE_SIZE_BYTES;
				return size;
				break;

			case JAUS_WM_OBJECT_RGB_DATA:
				size += JAUS_BYTE_SIZE_BYTES;	// Red
				size += JAUS_BYTE_SIZE_BYTES;	// Green
				size += JAUS_BYTE_SIZE_BYTES;	// Blue
				return size;
				break;

			default:
				return 0;			
		}
	}
	return size;
}

// JausWorldModelFeatureClassAttribute Copy
JausWorldModelFeatureClassAttribute featureClassAttributeCopy(JausWorldModelFeatureClassAttribute attribute)
{
	JausWorldModelFeatureClassAttribute copy;
	
	copy = (JausWorldModelFeatureClassAttribute) malloc(sizeof(JausWorldModelFeatureClassAttributeStruct));
	if(copy)
	{
		memcpy(copy, attribute, sizeof(JausWorldModelFeatureClassAttributeStruct));
		return copy;
	}
	else
	{
		return NULL;
	}
}

// JausWorldModelFeatureClassAttribute ToString
int featureClassAttributeToString(JausWorldModelFeatureClassAttribute attribute, char *string)
{
	int retVal = 0;

	switch(attribute->dataType)
	{
		case JAUS_WM_OBJECT_BYTE_DATA:
			retVal = sprintf(string, "Byte Attribute (%d)", attribute->byteValue);
			return retVal;
						
		case JAUS_WM_OBJECT_SHORT_DATA:
			retVal = sprintf(string, "Short Attribute (%d)", attribute->shortValue);
			return retVal;

		case JAUS_WM_OBJECT_INTEGER_DATA:
			retVal = sprintf(string, "Integer Attribute (%d)", attribute->integerValue);
			return retVal;

		case JAUS_WM_OBJECT_LONG_DATA:
			retVal = sprintf(string, "Long Attribute (%lld)", attribute->longValue);
			return retVal;

		case JAUS_WM_OBJECT_UNSIGNED_SHORT_DATA:
			retVal = sprintf(string, "Unsigned Short Attribute (%d)", attribute->unsignedShortValue);
			return retVal;

		case JAUS_WM_OBJECT_UNSIGNED_INTEGER_DATA:
			retVal = sprintf(string, "Unsigned Integer Attribute (%d)", attribute->unsignedIntegerValue);
			return retVal;

		case JAUS_WM_OBJECT_UNSIGNED_LONG_DATA:
			retVal = sprintf(string, "Unsigned Long Attribute (%lld)", attribute->unsignedLongValue);
			return retVal;

		case JAUS_WM_OBJECT_FLOAT_DATA:
			retVal = sprintf(string, "Float Attribute (%f)", attribute->floatValue);
			return retVal;

		case JAUS_WM_OBJECT_DOUBLE_DATA:
			retVal = sprintf(string, "Double Attribute (%lf)", attribute->doubleValue);
			return retVal;

		case JAUS_WM_OBJECT_RGB_DATA:
			retVal = sprintf(string, "RGB Attribute Red(%d) Green(%d) Blue(%d)", attribute->redValue, attribute->greenValue, attribute->blueValue);
			return retVal;

		default:
			return 0;
	}
}

// JausWorldModelFeatureClassAttribute GetValue
JausUnsignedLong featureClassAttributeGetValue(JausWorldModelFeatureClassAttribute attribute)
{
	JausUnsignedLong tempULong = 0;
	
	switch(attribute->dataType)
	{
		case JAUS_WM_OBJECT_BYTE_DATA:
			return (JausUnsignedLong)attribute->byteValue;
						
		case JAUS_WM_OBJECT_SHORT_DATA:
			return (JausUnsignedLong)attribute->shortValue;

		case JAUS_WM_OBJECT_INTEGER_DATA:
			return (JausUnsignedLong)attribute->integerValue;

		case JAUS_WM_OBJECT_LONG_DATA:
			return (JausUnsignedLong)attribute->longValue;

		case JAUS_WM_OBJECT_UNSIGNED_SHORT_DATA:
			return (JausUnsignedLong)attribute->shortValue;

		case JAUS_WM_OBJECT_UNSIGNED_INTEGER_DATA:
			return (JausUnsignedLong)attribute->integerValue;

		case JAUS_WM_OBJECT_UNSIGNED_LONG_DATA:
			return (JausUnsignedLong)attribute->unsignedLongValue;

		case JAUS_WM_OBJECT_FLOAT_DATA:
			return (JausUnsignedLong)attribute->floatValue;

		case JAUS_WM_OBJECT_DOUBLE_DATA:
			return (JausUnsignedLong)attribute->doubleValue;

		case JAUS_WM_OBJECT_RGB_DATA:
			tempULong = attribute->redValue << 16;
			tempULong += attribute->greenValue << 8;
			tempULong += attribute->blueValue;
			return tempULong;

		default:
			return 0;
	}
}
