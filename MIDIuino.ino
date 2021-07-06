#include <MIDI.h>
#include <MIDIUSB.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void noteOn(byte channel, byte pitch, byte velocity) {
  /*
  Serial.print("On:");
  Serial.print(channel);
  Serial.print(", ");
  Serial.print(pitch);
  Serial.print(", ");
  Serial.println(velocity);
  */
  
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  /*
  Serial.print("Off:");
  Serial.print(channel);
  Serial.print(", ");
  Serial.print(pitch);
  Serial.print(", ");
  Serial.println(velocity);
  */
  
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) { 
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


void setup()
{
    MIDI.begin();           // Launch MIDI, by default listening to channel 1.
    Serial.begin(9600);
}

static const uint8_t analog_pins[] = {A0,A1,A2,A3,A6,A7,A8,A9};
int pot_lastVal[8] = {0,0,0,0,0,0,0,0};
static const int CC[8] = {20,21,22,23,24,25,26,27};
uint8_t newAnalogValue;

void loop()
{
  if (MIDI.read())                // Is there a MIDI message incoming ?
  {
    /*
    Serial.println("Packet:");
    Serial.println(MIDI.getType());
    Serial.println(MIDI.getData1());
    Serial.println(MIDI.getData2());
    */
    byte type = MIDI.getType();
    byte channel = MIDI.getChannel();
    byte data1 = MIDI.getData1();
    byte data2 = MIDI.getData2();

    
    Serial.print(type);
    Serial.print(":");
    Serial.print(channel);
    Serial.print(":");
    Serial.print(data1);
    Serial.print(":");
    Serial.println(data2);
    
    
    /*
    254 - clock signal
    
    144 - key down
    128 - key up

    250 - start
    251 - guide
    252 - back
    192 
    */
    
    if(type == 144){
      noteOn(channel, data1, data2);
      MidiUSB.flush();
    }else if(type == 128){
      noteOff(channel, data1, data2);
      MidiUSB.flush();
    }else if(type == 254){
      
    }else{
      controlChange(channel, data1, data2);
      MidiUSB.flush();
    }
     
  }

  int val_clear = analogRead(A10);
  
  for (int i = 0; i < 8; ++i) {
    int val_temp_a = analogRead(analog_pins[i]);
    int val_temp_b = analogRead(analog_pins[i]);

    if(abs(val_temp_a - val_temp_b) < 20 && abs(val_temp_a - pot_lastVal[i]) > 30){
      newAnalogValue = val_temp_a >> 3;
      
      Serial.print("Knob:");
      Serial.print(i);
      Serial.print(", ");
      Serial.println(newAnalogValue);
      
      controlChange(1, CC[i], newAnalogValue);
      pot_lastVal[i] = val_temp_a;
    }
  }
  MidiUSB.flush();
  
}
