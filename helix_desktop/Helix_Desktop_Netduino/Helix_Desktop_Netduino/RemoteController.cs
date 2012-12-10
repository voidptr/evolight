using System;
using Microsoft.SPOT;
using NecDecoder;
using SecretLabs.NETMF.Hardware.Netduino;

namespace Helix
{
    class RemoteController
    {
        NecProtocolDecoder NecRemoteControlDecoder = new NecProtocolDecoder(Pins.GPIO_PIN_D7);

        // evo control
        public bool call_ga_reset;
        public bool call_pauseplay;
        public bool call_evostep;
        public bool call_raisemutrate;
        public bool call_lowermutrate;
        public bool call_togglemutation;
        public bool call_toggleselection;
        public bool call_toggleheredity;
        public bool call_mutationevent;

        // aesthetics
        public bool call_raisebrightness;
        public bool call_lowerbrightness;
        
        public RemoteController()
        {
            NecRemoteControlDecoder.OnIRCommandReceived += NecProtocolDecoder_OnIRCommandReceived;
            call_ga_reset = false;
        }

        void NecProtocolDecoder_OnIRCommandReceived(UInt32 irData)
        {
            //Debug.Print("IR Command Received: " + irData);

            switch (irData)
            {
                case 16580863:
                    Debug.Print("Vol Down");
                    Debug.Print("LOWER BRIGHTNESS");
                    call_lowerbrightness = true;
                    break;
                case 16613503:
                    Debug.Print("Play/Pause");
                    Debug.Print("PLAY/PAUSE GA");
                    call_pauseplay = true;
                    break;
                case 16597183:
                    Debug.Print("Vol Up");
                    Debug.Print("RAISE BRIGHTNESS");
                    call_raisebrightness = true;
                    break;
                case 16589023:
                    Debug.Print("Setup");
                    break;
                case 16621663:
                    Debug.Print("Prev (Up Arrow)");
                    Debug.Print("RAISE MUTATION RATE");
                    call_raisemutrate = true;
                    break;
                case 16605343:
                    Debug.Print("Stop/Mode");
                    break;
                case 16584943:
                    Debug.Print("Ch Down (Left Arrow)");
                    break;
                case 16617583:
                    Debug.Print("Enter/Save");
                    break;
                case 16601263:
                    Debug.Print("Ch Up (Right Arrow)");
                    Debug.Print("STEP FORWARD ONE GENERATION");
                    call_evostep = true;
                    break;
                case 16593103:
                    Debug.Print("0 10+");
                    Debug.Print("MUTATION EVENT");
                    call_mutationevent = true;
                    break;
                case 16625743:
                    Debug.Print("Next (Down Arrow)");
                    Debug.Print("LOWER MUTATION RATE");
                    call_lowermutrate = true;
                    break;
                case 16609423:
                    Debug.Print("Back (Twisty Arrow)");
                    Debug.Print("RESETTING GA");
                    call_ga_reset = true;
                    break;
                case 16582903:
                    Debug.Print("1");
                    break;
                case 16615543:
                    Debug.Print("2");
                    break;
                case 16599223:
                    Debug.Print("3");
                    break;
                case 16591063:
                    Debug.Print("4");
                    break;
                case 16623703:
                    Debug.Print("5");
                    break;
                case 16607383:
                    Debug.Print("6");
                    break;
                case 16586983:
                    Debug.Print("7");
                    Debug.Print("TOGGLE SELECTION");
                    call_toggleselection = true;
                    break;
                case 16619623:
                    Debug.Print("8");
                    Debug.Print("TOGGLE MUTATION");
                    call_togglemutation = true;
                    break;
                case 16603303:
                    Debug.Print("9");
                    Debug.Print("TOGGLE HEREDITY");
                    call_toggleheredity = true;
                    break;
            }
        }
    }
}
