


 
/*These are the PID Tuning variables.
* You can adjust these during operation by writing ki, kp, or kd and the appropriate value
*/
int kp=10,ki=2,kd=0.1;

//We'll use this for processing incoming data
String inputString = "";         
boolean stringComplete = false;
String action="";
String valueString="";
int value=0;


void setup(){
   //save some room for our inputs
   inputString.reserve(15);
   action.reserve(5);
   valueString.reserve(5);
   Serial.begin(9600);
   
   delay(400);
   Serial.print("Test Print, please ignore");
 }
 
 
 
 
 
 void loop(){
   //print our current status
   
   
   ////////////////////////
   //Process Serial command
   ////////////////////////
      
  if(stringComplete==true){
    //clean up data
    inputString.trim();
    //parse command
    int i=0;
    
    //grab some letters
    while(inputString[i]>='a'&& inputString[i]<='z'){action+=inputString[i++];}
    //strip spaces
    while(inputString[i]==' '){i++;}          
    //parse value 
    while(inputString[i]>='0'&& inputString[i]<='9'){valueString+=inputString[i++];}
     value=valueString.toInt();

     //optional debug print
     Serial.print("Action:");Serial.print(action);
     Serial.print("  Value: "); Serial.println(value);
     
     //now we've picked up the new data, so set the PID to do something
     if(action=="+"); //rotate X units
     if(action=="-");
     if(action=="kp"); //adjust the PID regulator
     if(action=="kd");
     if(action=="ki");
     if(action=="go"); //go to a specific value
     if(action=="set");
     if(action=="home"); //return home
     if(action=="off"); //turn off motor and PID regulator
     if(action=="on");
     if(action=="set");
     if(action=="?");//print lots of data!
     
     //reset everything for the next command
     inputString="";
     action="";
     valueString="";
     stringComplete=false;

  }
  
  
  //update our PID controller  
   
   
   
   
   delay(1000);
 }
 


// http://arduino.cc/en/Tutorial/SerialEvent
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
  
}

//*/
