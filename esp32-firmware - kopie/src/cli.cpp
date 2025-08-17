#include "cli.h"

// Create CLI Object
SimpleCLI cli;

// Commands
Command ping;
Command source;
Command out;
Command halt;
Command reset;
Command pd;

void setup_cli(){
  ping = cli.addCmd("ping");
  out = cli.addCmd("out");
  source = cli.addCmd("source");
  halt = cli.addCmd("halt");
  reset = cli.addCmd("reset");
  pd = cli.addCmd("pd");

  source.addArg("v", "NAN");
  source.addArg("i", "NAN");
  pd.addArg("v", "NAN");
}

void handle_cli(){
  // Check if user typed something into the serial monitor
  if (Serial.available()) {
    // Read out string from the serial monitor
    String input = Serial.readStringUntil('\n');

    // Echo the user input
    Serial.print("# ");
    Serial.println(input);

    // Parse the user input into the CLI
    cli.parse(input);
  }

  // Check for newly parsed commands
  if (cli.available()) {
    // Get command out of queue
    Command cmd = cli.getCmd();

    // React on our ping command
    if (cmd == ping) {
      Serial.println("Pong!");

    
    } else if (cmd == source) {
        Argument voltage = cmd.getArg("v");
        Argument current = cmd.getArg("i");
        if (!voltage.equals("NAN"))
        {
          converter.setVoltage( voltage.getValue().toFloat());
        }
        if (!current.equals("NAN"))
        {
          converter.setCurrentLimit( current.getValue().toFloat());
        }
        
    } else if (cmd == halt) {
      if (stopLoop == 0)
      {
        Serial.println("pausing loop");
        stopLoop = 1;
      }
      else
      {
        Serial.println("unpausing loop");
        stopLoop = 0;
      }
    } 
    else if (cmd == out) {
      if (converter.enabled)
      {
        Serial.println("Disabling output");
        converter.disable();
      }
      else
      {
        Serial.println("Enabling output");
        converter.enable();
      }
    } 
    else if (cmd == reset) {
      ESP.restart();
    } 
    else if (cmd == pd) {
        Argument voltage = cmd.getArg("v");
        if (!voltage.equals("NAN"))
        {
            usb.setPdoNumber(3); 
            Serial.print(F("Requesting USB-PD voltage:"));
            Serial.print(voltage.getValue().toInt());
            Serial.println();
            usb.setVoltage(3, voltage.getValue().toInt()); 
            usb.softReset(); // This forces re-negotiation
            usb.write();
        }
        
    }
  }

  // Check for parsing errors
  if (cli.errored()) {
    // Get error out of queue
    CommandError cmdError = cli.getError();

    // Print the error
    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    // Print correct command structure
    if (cmdError.hasCommand()) {
      Serial.print("Did you mean \"");
      Serial.print(cmdError.getCommand().toString());
      Serial.println("\"?");
    }
  }
}