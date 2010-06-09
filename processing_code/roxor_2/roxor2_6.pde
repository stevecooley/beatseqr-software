// HEY, modify 
int serial_device_number = 1;
int OSC_RECEIVE_PORT = 8112;
// int OSC_MIDI_CC_PORT = 8112;
int OSC_SEND_PORT = 8111;

int buildnumber = 6;

/**
 * Ubirox
 * the multiplatform successor to Roxor
 * 2009-10-29 steve cooley
 *
 * Ok, so we've all tried saying "ubirox" for about a month and it sort of blows, so I'm effectively dubbing this "Roxor 2.0"
 * 2009-12-03 steve cooley
 *
 * http://beatseqr.com
 */
import processing.serial.*;
import oscP5.*;
import netP5.*;
import controlP5.*;

ControlP5 controlP5;
PFont font;
OscP5 oscP5;
OscProperties properties = new OscProperties();

// NetAddress oscCC;
// NetAddress myRemoteLocation;

Serial serial_port;         // Create object from Serial class
String portName;
String serial_message;      // Data received from the serial port

String[] stepmap = {
  "0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};

int play = 0;

int patterns = 4;
int voices = 9;
int steps = 16;

int[][][] step_data =  new int[patterns][voices][steps];

int page_value;
int step_value;
int tempo = 120;
int tempoadjust = 0;
int swing = 0;


int myColorBackground = color(0,0,0);


// String message_name;
String serial_read_value;
String play_string = "ZPL";


void setup() {
  size(190,300);
  frameRate(25);

  controlP5 = new ControlP5(this);
  Radio r = controlP5.addRadio("radio",10,170);
  // r.setLabel("Click on your beatseqr serial device name");
  for(int i=0;i<Serial.list().length;i++) {
    r.add(Serial.list()[i],i);
  }

  controlP5.addNumberbox("sendport",OSC_SEND_PORT,10,130,50,14);
  controlP5.addNumberbox("receiveport",OSC_RECEIVE_PORT,70,130,50,14);


  font = loadFont("netlabel-square-ends-20.vlw");
  textFont(font, 20);

  //  oscP5 = new OscP5(this,OSC_RECEIVE_PORT);

  properties.setRemoteAddress("127.0.0.1",OSC_SEND_PORT);
  properties.setListeningPort(OSC_RECEIVE_PORT);
  oscP5 = new OscP5(this,properties);


  // hey, you don't have to do this anymore, sweet.
  // myRemoteLocation = new NetAddress("127.0.0.1",OSC_SEND_PORT);

  // this was a legacy bit of code to send the OSC messages for midi cc out to a different port.  Not needed anymore.
  //  oscCC = new NetAddress("0.0.0.0",OSC_MIDI_CC);

  portName = Serial.list()[serial_device_number];
  serial_port = new Serial(this, portName, 57600); 
  serial_port.clear();
  // catch and clear the first message in case it's garbage
  serial_read_value = serial_port.readStringUntil(59);
  serial_read_value = null;
  // println("from setup: serial_port.available = "+serial_port.available());


}


void draw() 
{
  // background(0);
  noStroke();
  fill(50,255);
  rect(0,0,width,height);
  smooth();  


  // fill(50);
  //  ellipse(int(random(width)), int(random(width)), int(random(width)), int(random(width)));

  while (serial_port.available() > 0) 
  {
    //println("serial_port.available = "+serial_port.available());
    serial_read_value = serial_port.readStringUntil(59);

    if (serial_read_value != null) {
      parse_message(serial_read_value);
      // println(serial_read_value);
    }



  }
  fill(255);

  text("Roxor version 2." + buildnumber, 10, 20);
  text("http://beatseqr.com", 10, 40);

  text("tempo : " + tempo, 10, 70);  
  text("tempo adjust : " + tempoadjust, 10, 90);    
  text("swing : " + swing, 10, 110);  

}








void radio(int theID) {

  // println("theID = "+theID);
  // println(Serial.list());
  serial_device_number = theID;
  portName = Serial.list()[serial_device_number];
  change_serial(theID);

}

void sendport(int thePortNumber)
{
  // println("sendport change: " + thePortNumber);
  OSC_SEND_PORT = thePortNumber;
  properties.setRemoteAddress("127.0.0.1",OSC_SEND_PORT);
  // println(properties.toString());
  OscMessage myMessage = new  OscMessage("/from_roxor");
  myMessage.add(thePortNumber);
  oscP5.send(myMessage);
}

void receiveport(int thePortNumber)
{
  // println("receiveport change: " + thePortNumber);
  OSC_RECEIVE_PORT = thePortNumber;
  properties.setListeningPort(OSC_RECEIVE_PORT);
  // println(properties.toString());

  OscMessage myMessage = new  OscMessage("/to_roxor");
  myMessage.add(thePortNumber);
  oscP5.send(myMessage);
}

/*
void controlEvent(ControlEvent theEvent) {
 // println("hi there" + theEvent.controller().id()+"  /  "+
 //   theEvent.controller()+"  /  "+
 //   int(theEvent.controller().value())
 //   );
 
 
 }
 */

void change_serial(int device_number)
{
  // println(device_number);

  portName = Serial.list()[device_number];

  serial_port.stop();  
  serial_port = new Serial(this, portName, 57600); 
  serial_port.clear();
  // catch and clear the first message in case it's garbage
  serial_read_value = serial_port.readStringUntil(59);
  serial_read_value = null;
  println("from setup: serial_port.available = "+serial_port.available() + " for port name" + portName);

}

void parse_message(String serial_read_value)
{
  String[] list = split(serial_read_value, ',');

  if(list.length == 5)
  {

    String thecommand = list[0];
    int thepage = int(list[1]);
    int thevoice = int(list[2]);
    // int thevoice = int(list[2])+1;
    int thestep = int(list[3]);

    String[] val = split(list[4], ';');
    int theval = int(val[0]);

    if(list[0].equals("ZSD"))
    {

      step_parser(thepage, thevoice, thestep, theval);

    }

  }

  if(list.length == 3)
  {
    String thecommand = list[0];
    int thekey = int(list[1]);
    String[] val = split(list[2], ';');
    int theval = int(val[0]);

    if(list[0].equals("ZNN")) // note nunmber
    {
      thekey = thekey+1;
      message("/midinotenum"+thekey, theval);
      // println("/midinotenum"+thekey +" "+ theval);
    }

    if(list[0].equals("ZVL")) // velocity
    {
      thekey = thekey+1;
      message("/velocity"+thekey, theval);
      // println("/velocity"+thekey +" "+ theval);
    }

    if(list[0].equals("ZCC")) // midi cc - general
    {
      thekey = thekey+1;
      message("/midicc"+thekey, theval);
      println("/midicc"+thekey +" "+ theval);
    }

    if(list[0].equals("ZMU")) // midi cc - mute
    {
      thekey = thekey+1;
      message("/mute"+thekey, theval);
      println("/mute"+thekey +" "+ theval);
    }

    if(list[0].equals("ZSO")) // midi cc - solo
    {
      thekey = thekey+1;
      message("/solo"+thekey, theval);
      println("/solo"+thekey +" "+ theval);
    }


    if(list[0].equals("ZPC")) // pattern copy
    {

      // message("/patterncopy"+thekey, theval);
      // println("/pattern"+thekey +" "+ theval);
      pattern_copy(thekey, theval);
    }

  }

  if(list.length == 2)
  { 
    // println(list[0] + " : " + list[1]);

    String thekey = list[0];
    String[] val = split(list[1], ';');

    int theval = int(val[0]);


    if(list[0].equals("ZPL"))
    {
      message("/play", theval);
      // println("/play " + theval);
    }

    if(list[0].equals("ZSW"))
    {
      message("/swing", theval);
      // println("/swing " + theval);
      swing = theval;
    }

    if(list[0].equals("ZTA"))
    {
      message("/tempoadjust", theval);
      tempoadjust = theval;
      // println("/tempoadjust " + theval);
    }    

    if(list[0].equals("ZTM"))
    {
      message("/tempo", theval);
      tempo = theval;

      // println("/tempo " + theval);
    }

    if(list[0].equals("ZPS"))
    {
      message("/patternselect", theval);
      //println("/patternselect " + theval);
      pattern_select(theval);  
    }    

    // println(thekey);
  }
  /*
  else
   {
   for(int i = 0; i <= list.length; i++)
   {
   print(list[i] + " : ");
   }
   println();
   }
   */


  return;
}


void step_parser(int thepage, int thevoice, int thestep, int theval)
{

  step_data[thepage][thevoice][thestep] = theval;

  // message("/matrix"+thevoice+stepmap[thestep], theval);

  step_message(thevoice, thestep, theval);

  // println("/matrix"+thevoice+stepmap[thestep]+" "+ theval);

}


void pattern_select(int pattern_number)
{
  // println("change to pattern " + (pattern_number+1));
  for(int i = 0; i<=7; i++)
  {

    for(int j = 0; j<=15; j++)
    {
      // message("/matrix"+" /"+i+" /"+stepmap[j], step_data[pattern_number][i][j]);
      step_message(i, j, step_data[pattern_number][i][j]);      
    } 


  }

  return;
}

void pattern_copy(int copy_from, int copy_to)
{
  for(int i = 0; i<=7; i++)
  {
    // print("pattern "+ copy_from + ": ");
    for(int j = 0; j<=15; j++)
    {
      // print(step_data[copy_from][i][j] + " ");
      step_data[copy_to][i][j] = step_data[copy_from][i][j];
    } 
    // println();

  }
  pattern_select(copy_to);
  // println("copy from " + (copy_from+1) + " to " + (copy_to+1));
  return;
}


void message(String message_name, int message_value)
{
  String the_message = "/beatseqr" + message_name; 
  OscMessage myMessage = new OscMessage(the_message);
  myMessage.add(message_value); /* add an int to the osc message */
  oscP5.send(myMessage); 

  // println(message_name);
  return;
}

void step_message(int outbound_row, int outbound_column, int outbound_step)
{
  String message_address = "/beatseqr/matrix" + "/" + outbound_row + "/" + outbound_column; 
  OscMessage myMessage = new OscMessage(message_address);
 // myMessage.add(outbound_column); /* add an int to the osc message */
 // myMessage.add(outbound_row); /* add an int to the osc message */
  myMessage.add(outbound_step); /* add an int to the osc message */
  oscP5.send(myMessage); 

  // println(message_name);
  return;
}


/*
void midicc_message(String message_name, int message_value)
 {
 OscMessage myMessage = new OscMessage(message_name);
 myMessage.add(message_value); 
 // add an int to the osc message
 oscP5.send(myMessage, oscCC); 
 }
 */


/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) 
{



  if(theOscMessage.checkAddrPattern("/currentstep")==true) 
  {
    if(theOscMessage.checkTypetag("i")) 
    {

      // disabled for reasons of stupidity

      int firstValue = theOscMessage.get(0).intValue();  
      // print("### received an osc message /currentstep with typetag i.");
      // println(" value: "+firstValue);
      serial_port.write(firstValue);

    }  

  }
  return;
}

