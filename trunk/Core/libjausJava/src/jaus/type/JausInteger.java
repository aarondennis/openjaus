// File Name: JausInteger.java
//
// Written By: Danny Kent (jaus@dannykent.com)
//
// Version: 3.2 
//
// Date: 07/04/06
//
// This file contains the Jaus Integer data type and its associated methods.  
// JausInteger is used to convert between the JAVA integer primitive and the JAUS type
//
// A Jaus Inetger is defined as a 32 bit signed integer

package jaus.type;

public class JausInteger
{
	private static final double RANGE = 4294967294.0; 	// NOTE: This range is incorrect, but is as defined by Jaus
	public static final int MAX_VALUE = 2147483647;
	public static final int MIN_VALUE = -2147483648;
	public static final int SIZE_BYTES = 4;
	
	private int value;
	
	public JausInteger()
	{
		value = 0;
	}

	public JausInteger(int value)
	{
		this.value = value;
	}

	public JausInteger(byte[] byteArray)
	{
		value = fromJausBuffer(byteArray, 0);
	}

	public JausInteger(byte[] byteArray, int index)
	{
		value = fromJausBuffer(byteArray, index);
	}
	
	public boolean toJausBuffer(byte[] byteArray)
	{
		return toJausBuffer(this.value, byteArray, 0);
	}
	
	public boolean toJausBuffer(byte[] byteArray, int index)
	{
		return toJausBuffer(this.value, byteArray, index);
	}

	public boolean setFromJausBuffer(byte[] byteArray)
	{
		return setFromJausBuffer(byteArray, 0);
	}
	
	public boolean setFromJausBuffer(byte[] byteArray, int index)
	{
		if(byteArray.length - index < SIZE_BYTES)
			return false;
		else
		{
			value = fromJausBuffer(byteArray, index);
			return true;
		}
	}
	
	public int getValue()
	{
		return value;
	}

	public void setValue(int value)
	{
		this.value = value;
	}
	
	public int size()
	{
		return SIZE_BYTES;
	}

	// Takes the input double and the scale range and stores the coresponding scaled integer in value
	// This is to support Jaus Scaled Integers
	// Real_Value = Integer_Value*Scale_Factor + Bias
	public double scaleToDouble(double min, double max)
	{
		//BUG: What to do when max < min
		//limit value between min and max Integer values
		if(value > MAX_VALUE) value = MAX_VALUE;
		if(value < MIN_VALUE) value = MIN_VALUE;
		
		double scaleFactor = (max-min)/RANGE;
		double bias = (max+min)/2.0; 
	    
		return value*scaleFactor + bias;
	}
	
	// Takes the value inside the object and returns it as a double in the range
	// This is to support Jaus Scaled Integers
	// Integer_Value = Round((Real_Value - Bias)/Scale_Factor)
	public void setFromDouble(double value, double min, double max)
	{
		//BUG: What to do when max < min
		double scaleFactor = (max-min)/RANGE;
		double bias = (max+min)/2.0;
		
		// limit real number between min and max
		if(value < min) value = min;
		if(value > max) value = max;
		
		// calculate rounded integer value
		this.value = (int) Math.round((value - bias)/scaleFactor);
	}
	
	public static boolean toJausBuffer(int value, byte[] byteArray)
	{
		return toJausBuffer(value, byteArray, 0);
	}
	
	public static boolean toJausBuffer(int value, byte[] byteArray, int index)
	{
		if(byteArray.length < index + SIZE_BYTES)
			return false; //not enough size
		
		for(int i=0; i < SIZE_BYTES; i++)
		{
			byteArray[index+i] = (byte) ((value >>> i*8) & 0xFF); // 8 bits per byte
		}
		return true;
	}
	
	public static int fromJausBuffer(byte[] byteArray)
	{
		return fromJausBuffer(byteArray, 0);
	}
	
	public static int fromJausBuffer(byte[] byteArray, int index)
	{
		int tempValue = 0;
		int arrayUpperBound = byteArray.length-index;
		// NOTE: Should not get an input array not equal in size to a Integer
		//       If this occurs, the higher order bytes will be left as zeros 
		for(int i = 0; i < SIZE_BYTES && i < arrayUpperBound; i++)
		{
			tempValue += ((byteArray[index+i] & 0xFF) << i*8); // 8 bits per byte
		}
		
		return tempValue;
	}
	
	public String toHexString()
	{
		String temp = "";
		temp = Integer.toHexString(value).toUpperCase();
	
		for(int i=temp.length(); i < (SIZE_BYTES*2); i++)
		{
			temp = "0"+temp;
		}
		return temp;
	}
	
	public String toString()
	{
		return Integer.toString(this.value);
	}
}
