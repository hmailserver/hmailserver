/*
******************************************************************
*  Form Check, JavaScript Form Validation Library
******************************************************************
*
*  FormCheck currently supports the following contols:
*	- Null values
*	- Minimum and maximum lengths
*	- Minimum and maximum values (for numbers)
*	- Type controls
*
*  FormCheck currently supports the following input types:
*	- date (mm/dd/yyyyy)
*	- datemysql (yyyy-mm-dd)
*	- email
*	- ipaddress
*	- number
*	- text
*
*/
function securityCheck(input) {
	/*
	  SecurityCheck Function
	*/
	var badchars	= Array('\'','\"','%','\\');
	for(var i=0; i<badchars.length;i++) {
		if(input.indexOf(badchars[i]) != -1)
		  return false;
	}
}

function inputCheck(input,type) {
	/*
	  InputCheck Function
	*/
	var errors = 0;
	var expression;

	switch(type) {
		case 'email':
			expression	= /^.+\@(\[?)[a-zA-Z0-9\-\.]+\.([a-zA-Z]{2,}|[0-9]{1,3})(\]?)$/;
		break;
		case 'text':
			expression	= "";
		break;
		case 'number':
			expression	= /^[0-9]+$/;
		break;
		case 'date':
			expression	= /^\d{1,2}(\-|\/|\.)\d{1,2}\1\d{4}$/;
		break;
		case 'datedb':
			expression	= /^([0-9]{4})-([0-9]{1,2})-([0-9]{1,2})$/;
		break;
		case 'datemysql':
			expression	= /^([0-9]{4})-([0-9]{1,2})-([0-9]{1,2})$/;
		break;
		case 'ipaddress':
			expression	= /^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$/;
		break;
		case '':
		break;
	}

	if((errors == 0) && (expression != '')) {
		if(!expression.exec(input))
		  errors++;
	}

	if(errors==0)
		return true;
	else
		return false;
}

function formCheck(cform) {
	/*
	  FormCheck Function
	*/
	if(!cform) {
		alert("Empty form object detected!");
		return false;
	}

	var errors		= 0;
	var message		= "Invalid input for:\n";
	var controlmessages	= Array();

	if(cform.formmessage)
	{
		message = cform.formmessage + "\n";
	}
	
	
	for(i=0;i<cform.elements.length;i++) {
		// Get manual message for this input
		if(cform.elements[i].getAttribute('checkmessage'))
		  controlmessages[i] = cform.elements[i].getAttribute('checkmessage');
		else
		  controlmessages[i] = "";
		// Get message

		// 1. NULL CONTROL
		if(cform.elements[i].getAttribute('checkallownull') && cform.elements[i].getAttribute('checkallownull')=='false') {
			if(cform.elements[i].value) 
			{
				if((cform.elements[i].value == '') || (cform.elements[i].value == ' ')) {
					errors++;
					if(controlmessages[i] != "")
				  	  message +="* "+controlmessages[i]+"\n";
					else
				 	  message +="* Field "+cform.elements[i].name+" cannot be blank.\n";
				}
			} else {
				errors++;
				if(controlmessages[i] != "")
				  message +="* "+controlmessages[i]+"\n";
				else
				  message +="* Field "+cform.elements[i].name+" cannot be undefined.\n";
			}
		}
		// END OF NULL CONTROL

		// 2. TYPE CONTROL
		if((cform.elements[i].getAttribute('checktype')) && (cform.elements[i].value)) 
		{
			if(!inputCheck(cform.elements[i].value,cform.elements[i].getAttribute('checktype'))) {
				errors++;
				if(controlmessages[i] != "")
				  message +="* "+controlmessages[i]+"\n";
				else
				  message +="* Invalid "+cform.elements[i].checktype+" format in "+cform.elements[i].name+".\n";
			}
		}
		// END OF TYPE CONTROL

		// 3. LENGTH CONTROL
		if((cform.elements[i].getAttribute('checkminlen')) && (cform.elements[i].value)) {
			if(cform.elements[i].value.length < cform.elements[i].getAttribute('checkminlen')) {
				errors++;
				if(controlmessages[i] != "")
				  message +="* "+controlmessages[i]+"\n";
				else
				  message +="* Length of "+cform.elements[i].name+" cannot be less than "+cform.elements[i].checkminlen+" characters.\n";
			}
		}
		if((cform.elements[i].getAttribute('checkmaxlen')) && (cform.elements[i].value)) {
			if(cform.elements[i].value.length > cform.elements[i].getAttribute('checkmaxlen')) {
				errors++;
				if(controlmessages[i] != "")
				  message +="* "+controlmessages[i]+"\n";
				else
				  message +="* Length of "+cform.elements[i].name+" cannot be more than "+cform.elements[i].checkmaxlen+" characters.\n";
			}
		}
		// END OF LENGTH CONTROL

		// 3. MIN/MAX VALUE CONTROL FOR NUMBERS
		if((cform.elements[i].getAttribute('checkminvalue')) && (cform.elements[i].value)) {
			if(cform.elements[i].value < cform.elements[i].getAttribute('checkminvalue')) {
				errors++;
				if(controlmessages[i] != "")
				  message +="* "+controlmessages[i]+"\n";
				else
				  message +="* Value of "+cform.elements[i].name+" cannot be less than "+cform.elements[i].checkminvalue+".\n";
			}
		}
		if((cform.elements[i].getAttribute('checkmaxvalue')) && (cform.elements[i].value)) {
			if(cform.elements[i].value > cform.elements[i].getAttribute('checkmaxvalue')) {
				errors++;
				if(controlmessages[i] != "")
				  message +="* "+controlmessages[i]+"\n";
				else
				  message +="* Value of "+cform.elements[i].name+" cannot be less than "+cform.elements[i].checkmaxvalue+".\n";
			}
		}
		// END OF MIN/MAX VALUE CONTROL
	} // END OF THE FOR LOOP

	if(errors == 0) {
	  return true;
	} else {
	  alert(message);
	  return false;
	}
}
