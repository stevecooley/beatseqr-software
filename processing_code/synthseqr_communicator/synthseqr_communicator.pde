// HEY, modify 
int serial_device_number = 2;
int OSC_RECEIVE_PORT = 8118;
// int OSC_MIDI_CC_PORT = 8112;
int OSC_SEND_PORT = 8117;

int buildnumber = 12;

import controlP5.*;
ControlP5 controlP5;


import processing.serial.*;
import oscP5.*;
import netP5.*;

int transportState = 0; // not playing

CheckBox step_program;
CheckBox playing_step;

CheckBox patternSelect;
RadioButton r;
RadioButton serialList;


PFont font;
OscP5 oscP5;
OscProperties properties = new OscProperties();

// NetAddress oscCC;
// NetAddress myRemoteLocation;

Serial serial_port;         // Create object from Serial class
String portName;
String serial_message;      // Data received from the serial port

String theControllerEventing;

String[] stepmap = {
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"
};

int play = 0;

int patterns = 4;
int voices = 1;
int steps = 16;

int nnnumber;
int ccnumber;
int vlnumber;

int[][][] step_data =  new int[patterns][voices][steps];

int page_value;
int step_value;
int tempo = 120;
int tempoadjust = 0;
int octaveadjust = 0;
int swing = 0;


int myColorBackground = color(0, 0, 0);


// String message_name;
String serial_read_value;
String play_string = "ZPL";

CheckBox t;

void setup() {
  size(600, 350);
  frameRate(25);

  controlP5 = new ControlP5(this);






  Slider sliders; 



  patternSelect  = controlP5.addCheckBox("pattern_select", 10, 330);
  patternSelect.setItemsPerRow(4);
  patternSelect.setSpacingColumn(30);
  patternSelect.addItem("P1", 0);
  patternSelect.addItem("P2", 1);
  patternSelect.addItem("P3", 2);
  patternSelect.addItem("P4", 3);

  // println(r);

  RadioButton serialList = controlP5.addRadioButton("serial_list")
                                    .setPosition(10, 220);

  // r.setLabel("Click on your beatseqr serial device name");
  for (int i=0;i<Serial.list().length;i++) {
    serialList.addItem(Serial.list()[i], i);
  }

  step_program = controlP5.addCheckBox("step_program", 190, 330);  

  // make adjustments to the layout of a checkbox.
  step_program.setColorForeground(color(200));
  step_program.setColorActive(color(255));
  step_program.setColorLabel(color(200));
  step_program.setItemsPerRow(16);
  step_program.setSpacingColumn(14);
  step_program.setSpacingRow(10);

  playing_step = controlP5.addCheckBox("playing_step", 190, 315);  

  playing_step.setColorForeground(color(200));
  playing_step.setColorActive(color(255));
  playing_step.setColorLabel(color(200));
  playing_step.setItemsPerRow(16);
  playing_step.setSpacingColumn(14);
  playing_step.setSpacingRow(10);

  // add items to a checkbox.


  for (int i = 0; i<=(steps-1); i++)
  {
    int j = i * 25;
    // Slider(theName, float theMin, float theMax, float theDefaultValue, int theX, int theY, int theWidth, int theHeight) 

    // velocity

    sliders = controlP5.addSlider("vl"+i, 0, 128, 0, 190+j, 20, 10, 75);
    Slider thisSlider = (Slider)controlP5.controller("vl"+(i));
    thisSlider.setDecimalPrecision(0);
    thisSlider.setSliderMode(Slider.FLEXIBLE);

    // midi CC
    sliders = controlP5.addSlider("cc"+i, 0, 128, 0, 190+j, 120, 10, 75);
    Slider ccSlider = (Slider)controlP5.controller("cc"+(i));
    ccSlider.setDecimalPrecision(0);
    ccSlider.setSliderMode(Slider.FLEXIBLE);

    // midi note number
    sliders = controlP5.addSlider("nn"+i, 0, 128, 0, 190+j, 220, 10, 75);
    Slider nnSlider = (Slider)controlP5.controller("nn"+(i));
    nnSlider.setDecimalPrecision(0);
    nnSlider.setSliderMode(Slider.FLEXIBLE);


    t = step_program.addItem("s"+i, i);
    t.captionLabel().setColorBackground(color(80));
    t.captionLabel().style().movePadding(2, 0, 0, 2);
    t.captionLabel().style().moveMargin(-2, 0, 0, -3);

    CheckBox p = playing_step.addItem("x"+i, i);

    // step_program.addItem("s"+i, i);
  }  

  controlP5.addNumberbox("sendport", OSC_SEND_PORT, 10, 180, 50, 14);
  controlP5.addNumberbox("receiveport", OSC_RECEIVE_PORT, 70, 180, 50, 14);

  /* custom setup buttons... these make it easy to snap into a custom set of channels and note numbers */

  // 2012-06-08 steve cooley
  // let's.. not do this either.
  /*
  controlP5.addButton("iElectribe", 10, 10, 120, 80, 19);
  controlP5.addButton("Reason", 10, 100, 120, 80, 19);
  controlP5.addButton("iMS20_full", 10, 10, 145, 80, 19);
  controlP5.addButton("iMS20_synth", 10, 100, 145, 80, 19);
  */
  
  controlP5.addButton("play", 10, 10, 290, 80, 19);

  font = loadFont("netlabel-square-ends-20.vlw");
  textFont(font, 20);

  //  oscP5 = new OscP5(this,OSC_RECEIVE_PORT);

  properties.setRemoteAddress("127.0.0.1", OSC_SEND_PORT);
  properties.setListeningPort(OSC_RECEIVE_PORT);
  oscP5 = new OscP5(this, properties);


  // hey, you don't have to do this anymore, sweet.
  // myRemoteLocation = new NetAddress("127.0.0.1",OSC_SEND_PORT);

  // this was a legacy bit of code to send the OSC messages for midi cc out to a different port.  Not needed anymore.
  //  oscCC = new NetAddress("0.0.0.0",OSC_MIDI_CC);

  // portName = Serial.list()[serial_device_number];
  serial_port = new Serial(this, portName, 57600); 
  serial_port.clear();
  // catch and clear the first message in case it's garbage
  serial_read_value = serial_port.readStringUntil(59);
  serial_read_value = null;
  // println("from setup: serial_port.available = "+serial_port.available());

  pattern_select(0);
}


void draw() 
{
  // background(0);
  noStroke();
  fill(50, 255);
  rect(0, 0, width, height);
  smooth();  


  // fill(50);
  //  ellipse(int(random(width)), int(random(width)), int(random(width)), int(random(width)));

  while (serial_port.available () > 0) 
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

/*
public void iElectribe(int theValue) {

  for (int i = 1; i <=8; i++)
  { 
    message("/midichannel/"+i, 10);
  }

  message("/midinotenum/1", 36);
  message("/midinotenum/2", 38);
  message("/midinotenum/3", 40);
  message("/midinotenum/4", 41);
  message("/midinotenum/5", 42);
  message("/midinotenum/6", 46);
  message("/midinotenum/7", 49);
  message("/midinotenum/8", 39);
}


public void Reason(int theValue) {

  for (int i = 1; i <=8; i++)
  { 
    message("/midichannel/"+i, 1);
  }

  message("/midinotenum/1", 36);
  message("/midinotenum/2", 37);
  message("/midinotenum/3", 38);
  message("/midinotenum/4", 39);
  message("/midinotenum/5", 40);
  message("/midinotenum/6", 41);
  message("/midinotenum/7", 42);
  message("/midinotenum/8", 43);
}

public void iMS20_full(int theValue) {

  for (int i = 1; i <=8; i++)
  { 
    message("/midichannel/"+i, i);
  }

  message("/midinotenum/1", 36);
  message("/midinotenum/2", 38);
  message("/midinotenum/3", 40);
  message("/midinotenum/4", 41);
  message("/midinotenum/5", 42);
  message("/midinotenum/6", 46);
  message("/midinotenum/7", 49);
  message("/midinotenum/8", 39);
}

public void iMS20_synth(int theValue) {

  for (int i = 1; i <=8; i++)
  { 
    message("/midichannel/"+i, 1);
    message("/midinotenum/"+i, i+35);
  }
}

*/

public void play(int theValue) {

  if (transportState == 0)
  {
    message("/play", 1);
    transportState = 1;
  }
  else
  {
    message("/play", 0);
    transportState = 0;
  }
}

void serial_list(int theID) {

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
  properties.setRemoteAddress("127.0.0.1", OSC_SEND_PORT);
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
  delay(100);
  serial_port = new Serial(this, portName, 57600); 
  serial_port.clear();
  // catch and clear the first message in case it's garbage
  serial_read_value = serial_port.readStringUntil(59);
  serial_read_value = null;
  // println("from setup: serial_port.available = "+serial_port.available() + " for port name" + portName);
}

void parse_message(String serial_read_value)
{
 // println(serial_read_value);
  String[] list = split(trim(serial_read_value), ',');

  println(list);

  if (list.length == 5)
  {

    String thecommand = list[0];
    int thepage = int(list[1]);
    int thevoice = int(list[2]);
    // int thevoice = int(list[2])+1;
    int thestep = int(list[3]);

    String[] val = split(list[4], ';');
    int theval = int(val[0]);

    if (list[0].equals("ZSD"))
    {

      step_parser(thepage, thevoice, thestep, theval);
    }
  }

  if (list.length == 3)
  {
    String thecommand = list[0];
    int thekey = int(list[1]);
    String[] val = split(list[2], ';');
    int theval = int(val[0]);

    if (list[0].equals("ZMC")) // midi channel
    {
      thekey = thekey+1;
      message("/midichannel/1/"+thekey, theval);
      // println("/midinotenum"+thekey +" "+ theval);
    }

    if (list[0].equals("ZNN")) // note nunmber
    {
      thekey = thekey+1;
      message("/midinn/voice1/"+thekey, theval);
      // println("/midinotenum"+thekey +" "+ theval);
      controlP5.controller("nn"+(thekey-1)).setValue(theval);
    }

    if (list[0].equals("ZVL")) // velocity
    {
      thekey = thekey+1;
      message("/midivl/voice1/"+thekey, theval);
      controlP5.controller("vl"+(thekey-1)).setValue(theval);
      // println("/velocity"+thekey +" "+ theval);
    }

    if (list[0].equals("ZCC")) // midi cc - general
    {
      thekey = thekey+1;
      message("/midicc/voice1/"+thekey, theval);
      // println("/midicc/"+thekey +" "+ theval);
      controlP5.controller("cc"+(thekey-1)).setValue(theval);
    }

    /*  
    // 2012-06-08 steve cooley
    // Let's.... not do this.
        
    if (list[0].equals("ZRN")) // record note nunmber
    {
      thekey = thekey+1;
      message("/record/notenum/1/"+thekey, theval);
      // println("/midinotenum"+thekey +" "+ theval);
    }

    if (list[0].equals("ZRV")) // record velocity
    {
      thekey = thekey+1;
      message("/record/velocity/1/"+thekey, theval);
      // println("/velocity"+thekey +" "+ theval);
    }

    if (list[0].equals("ZRC")) // record midi cc - general
    {
      thekey = thekey+1;
      message("/record/cc/1/"+thekey, theval);
      // println("/midicc/"+thekey +" "+ theval);
    }


    if (list[0].equals("ZMU")) // midi cc - mute
    {
      thekey = thekey+1;
      message("/mute/"+thekey, theval);
      // println("/mute/"+thekey +" "+ theval);
    }

    if (list[0].equals("ZSO")) // midi cc - solo
    {
      thekey = thekey+1;
      message("/solo/"+thekey, theval);
      // println("/solo/"+thekey +" "+ theval);
    }


    if (list[0].equals("ZPC")) // pattern copy
    {

      // message("/patterncopy"+thekey, theval);
      // println("/pattern"+thekey +" "+ theval);
      pattern_copy(thekey, theval);
    }
  */
  }

  if (list.length == 2)
  { 
    // println(list[0] + " : " + list[1]);

    String thekey = list[0];
    String[] val = split(list[1], ';');

    int theval = int(val[0]);


    if (list[0].equals("ZPL"))
    {
      message("/play", theval);
      println("/play " + theval);
    }

    if (list[0].equals("ZRE"))
    {
      message("/arm_recording", theval);
      // println("/play " + theval);
    }


    if (list[0].equals("ZSW"))
    {
      message("/swing", theval);
      // println("/swing " + theval);
      swing = theval;
    }

    if (list[0].equals("ZTA"))
    {
      message("/tempoadjust", theval);
      tempoadjust = theval;
      // println("/tempoadjust " + theval);
    }    

    if (list[0].equals("ZTM"))
    {
      message("/tempo", theval);
      tempo = theval;

      // println("/tempo " + theval);
    }


    if (list[0].equals("ZOC"))
    {
      message("/octaveadjust", theval);
      octaveadjust = theval;
      // println("/octaveadjust " + theval);
    }    


    if (list[0].equals("ZPS"))
    {
      message("/patternselect", theval);
      // println("/patternselect " + theval);
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

void playing_step(int step_number)
{
  // println("playing step " + (step_number));

  // deactivate all pattern checkboxes on the interface, then activate the selected pattern.

  // set up buttons on the Roxor interface
  playing_step.deactivateAll();
  playing_step.toggle(step_number);
}


void pattern_select(int pattern_number)
{
  // println("change to pattern " + (pattern_number+1));

  // deactivate all pattern checkboxes on the interface, then activate the selected pattern.

  // set up buttons on the Roxor interface
  patternSelect.deactivateAll();
  patternSelect.toggle(pattern_number);  

  int current_step_value = 0;

  // data setup and transmit to steppa
  for (int i = 0; i<=(voices-1); i++)
  {

    for (int j = 0; j<=(steps-1); j++)
    {
      // message("/matrix"+" /"+i+" /"+stepmap[j], step_data[pattern_number][i][j]);

      current_step_value = step_data[pattern_number][i][j];
      if (current_step_value == 1)
      {
        // println("hi there step prgram on");
        // step_program.activate(j);
      }
      else
      {
        // step_program.deactivate(j);
        // println("Step program off");
      }

      step_message(i, j, step_data[pattern_number][i][j]);

      // println("pattern select " + i + " " + j + " value " + current_step_value);
    }
  }

  return;
}

void pattern_copy(int copy_from, int copy_to)
{
  for (int i = 0; i<=(voices-1); i++)
  {
    // print("pattern "+ copy_from + ": ");
    for (int j = 0; j<=(steps-1); j++)
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

  // println("step message row " + outbound_row + " col " + outbound_column + " value " + outbound_step);

  /*
  // controlP5.controller("cc"+(thekey-1)).setValue(theval);
   if (outbound_step == 1)
   {
   step_program.activate(outbound_column);
   println("step_message step program on");
   }
   else if (outbound_step == 0)
   {
   step_program.deactivate(outbound_column);
   println("step_message step program off");
   
   }
   */
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



  if (theOscMessage.checkAddrPattern("/currentstep")==true) 
  {
    if (theOscMessage.checkTypetag("i")) 
    {

      // disabled for reasons of stupidity

      int firstValue = theOscMessage.get(0).intValue();  
      // print("### received an osc message /currentstep with typetag i.");
      // println(" value: "+firstValue);
      serial_port.write(firstValue);
      playing_step(firstValue);
    }
  }
  return;
}



public void controlEvent(ControlEvent theEvent) {


  if (theEvent.isGroup()) {

    theControllerEventing = theEvent.group().name();

    if (theControllerEventing == "step_program")
    {
      // println(theEvent.group().name());
      // println(theEvent.group().arrayValue());
      for (int i=0;i<theEvent.group().arrayValue().length;i++) {
        // print(int(theEvent.group().arrayValue()[i]));
        step_message(0, i, int(theEvent.group().arrayValue()[i]));
      }
    }
    else if (theControllerEventing == "playing_step")
    {
      //
    }
    else {
      // println(" an event from controller group "+theEvent.controller().name());
    }
    //  println(theEvent.group());
  }
  else
  { // just a regular controller not in a group

    theControllerEventing = theEvent.controller().name();

    if (theControllerEventing.equals("nn0"))
    {
      //  message("/midinotenum/voice1", notenumber0);
    }
    else // none of the above, so say something.
    {

      // println("this got an event from "+theEvent.controller().name());
    }
  }
}

/*
public void step_program(ControlEvent theEvent) {
 
 println("!!got an event from "+theEvent.group().name());
 }
 */

public void addToRadioButton(RadioButton theRadioButton, String theName, int theValue ) {
  RadioButton t = theRadioButton.addItem(theName, theValue);
  t.captionLabel().setColorBackground(color(80));
  t.captionLabel().style().movePadding(2, 0, -1, 2);
  t.captionLabel().style().moveMargin(-2, 0, 0, -3);
}

