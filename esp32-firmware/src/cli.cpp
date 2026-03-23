#include "cli.h"
#include "display.h"

// Serial CLI instance (named to avoid shadowing the "cli" module)
SimpleCLI serialCli;

// Command handles
Command cmdPing;
Command cmdSource;
Command cmdOut;
Command cmdHalt;
Command cmdReset;
Command cmdPd;

void setup_cli()
{
  cmdPing = serialCli.addCmd("ping");
  cmdOut = serialCli.addCmd("out");
  cmdSource = serialCli.addCmd("source");
  cmdHalt = serialCli.addCmd("halt");
  cmdReset = serialCli.addCmd("reset");
  cmdPd = serialCli.addCmd("pd");

  cmdSource.addArg("v", "NAN");
  cmdSource.addArg("i", "NAN");
  cmdPd.addArg("v", "NAN");
}

void handle_cli()
{
  // Check if user typed something into the serial monitor
  if (Serial.available())
  {
    // Read out string from the serial monitor
    String input = Serial.readStringUntil('\n');

    // Echo the user input
    Serial.print("# ");
    Serial.println(input);

    // Parse the user input into the CLI
    serialCli.parse(input);
  }

  // Check for newly parsed commands
  if (serialCli.available())
  {
    // Get command out of queue
    Command cmd = serialCli.getCmd();

    if (cmd == cmdPing)
    {
      Serial.println("Pong!");
    }
    else if (cmd == cmdSource)
    {
      Argument voltage = cmd.getArg("v");
      Argument current = cmd.getArg("i");
      if (!voltage.equals("NAN"))
      {
        setSourceVoltage(voltage.getValue().toFloat());
      }
      if (!current.equals("NAN"))
      {
        setSourceCurrent(current.getValue().toFloat());
      }
    }
    else if (cmd == cmdHalt)
    {
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
    else if (cmd == cmdOut)
    {
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
    else if (cmd == cmdReset)
    {
      ESP.restart();
    }
    else if (cmd == cmdPd)
    {
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
  if (serialCli.errored())
  {
    // Get error out of queue
    CommandError cmdError = serialCli.getError();

    // Print the error
    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    // Print correct command structure
    if (cmdError.hasCommand())
    {
      Serial.print("Did you mean \"");
      Serial.print(cmdError.getCommand().toString());
      Serial.println("\"?");
    }
  }
}