//File Name: QueryComponentStatusMessage.java
//
// Written By: Tom Galluzzo (galluzzo@gmail.com)
//
// Version: 3.2  
//
// Date: 07/04/06
//

package jaus.message.query;
import jaus.message.*;

public class QueryComponentStatusMessage extends JausMessage
{    
	private static final int COMMAND_CODE = QUERY_COMPONENT_STATUS;
	private static final int MAX_DATA_SIZE_BYTES = 0;
    
	public QueryComponentStatusMessage()
	{
		super();
	}
	
	public QueryComponentStatusMessage(byte[] buffer)
	{
		super(buffer, 0);
	}
	
	public QueryComponentStatusMessage(byte[] buffer, int index)
	{
	    super(buffer, index);
	}

	public QueryComponentStatusMessage(JausMessage jausMessage)
	{
		super(jausMessage);
	}
	
	public void setCommandCode(int commandCode)
	{
	    this.commandCode = COMMAND_CODE;
	}	
	
}