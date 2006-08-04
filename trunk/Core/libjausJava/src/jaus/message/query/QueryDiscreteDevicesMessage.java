//File Name: QueryDiscreteDevicesMessage.java
//
// Written By: Danny Kent (jaus@dannykent.com)
//
// Version: 3.2  
//
// Date: 07/04/06
//

package jaus.message.query;
import jaus.message.*;
import jaus.type.JausByte;

public class QueryDiscreteDevicesMessage extends JausMessage
{    
	private static final int COMMAND_CODE = QUERY_DISCRETE_DEVICES;
	private static final int MAX_DATA_SIZE_BYTES = 1;
	
	public static final int MAIN_PROPULSION_BIT = 0;
	public static final int PARKING_BRAKE_HORN_BIT = 1;
	public static final int GEAR_BIT = 2;
	public static final int TRANSFER_CASE_BIT = 3;
    
	//Message Fields
	JausBytePresenceVector vector;
		
	public QueryDiscreteDevicesMessage()
	{
		super();
		vector = new JausBytePresenceVector(); 
	}
	
	public QueryDiscreteDevicesMessage(byte[] buffer)
	{
		super(buffer, 0);
	}
	
	public QueryDiscreteDevicesMessage(byte[] buffer, int index)
	{
	    super(buffer, index);
	}

	public QueryDiscreteDevicesMessage(JausMessage jausMessage)
	{
	    super(jausMessage);
	    vector = new JausBytePresenceVector(data);
	}
	
	public boolean setPresenceVectorBit(int bit)
	{
		return vector.setBit(bit);
	}
	
	public boolean clearPresenceVectorBit(int bit)
	{
		return vector.clearBit(bit);
	}
	
	public JausByte getPresenceVector()
	{
		return vector.getPresenceVector();
	}
	
	
	// Takes a data UDP or regular JAUS buffer and unpacks it into the header data structure and the data attributes
	public boolean setFromJausBuffer(byte[] buffer, int index)
	{
		if(super.setFromJausBuffer(buffer, index))
		{
			vector.setPresenceVector(new JausByte(data));
			return true;
		}
		else
			return false;
	}
	
	//TODO: Ask Tom about these messages, something doesn't seem right, but I might just be tired
	
	// Takes the header and data byte array and packs them into a data buffer
	// This method needs to be overridden for all subclasses of JausMessage to reflect the correct pack routine
	public boolean toJausBuffer(byte[] buffer, int index)
	{
	    if(super.toJausBuffer(buffer, index))
	    {
	    	index += HEADER_SIZE_BYTES;
	    	
	    	if(vector.getPresenceVector().toJausBuffer(buffer, index))
	    	{
	    		index += vector.size();
	    	}
	    	else return false;
	    	
	    	return true;
	    }
	    else
	    	return false;
	}
	
	// Overloaded method to accept a buffer and pack this message into its UDP form
	public boolean toJausUdpBuffer(byte[] buffer, int index)
	{
	    if(super.toJausUdpBuffer(buffer, index))
	    {
	    	index += HEADER_SIZE_BYTES + UDP_HEADER_SIZE_BYTES;
			
	    	if(vector.getPresenceVector().toJausBuffer(buffer, index))
	    	{
	    		index += vector.size();
	    	}
	    	else return false;
	    	
	    	return true;
	    }
	    else
	    	return false;
			
	}
}