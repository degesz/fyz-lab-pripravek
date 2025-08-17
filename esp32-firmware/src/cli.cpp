#include "cli.h"

// Create CLI Object
SimpleCLI cli;

// Commands
Command ping;

Command out;
Command halt;
Command reset;

void setup_cli(){
  ping = cli.addCmd("ping");
  out = cli.addCmd("out");
  halt = cli.addCmd("halt");
  reset = cli.addCmd("reset");

  out.addArg("v", "NAN");
  out.addArg("i", "NAN");
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

    
    } else if (cmd == out) {
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
    else if (cmd == reset) {
      ESP.restart();
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