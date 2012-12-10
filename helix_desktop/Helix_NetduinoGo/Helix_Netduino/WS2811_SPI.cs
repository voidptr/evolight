using Microsoft.SPOT.Hardware; 

namespace WS2811_SPI
{
    public class WS2811_SPI
    {
        
        int m_num_channels; 
        Cpu.Pin[] m_pins;
        int[] m_lights_per_channel;
        int m_totallights;
        int[] m_lights_start;
        int[] m_lights_end;
        
        SPI.Configuration[] SPIConfigs;
        SPI SPIBus;
        
        byte[][] m_lights;
        
        public WS2811_SPI(int num_channels, Cpu.Pin[] pins, int[] lights_per_channel)
        {
            
            SPIConfigs = new SPI.Configuration[num_channels];
            
            m_num_channels = num_channels;
            m_pins = pins;
            m_lights_per_channel = lights_per_channel;

            m_lights_start = new int[m_num_channels];
            m_lights_end = new int[m_num_channels];
            
            int lights = 0;
            for (int i = 0; i < m_num_channels; i++)
            {
                m_lights_start[i] = lights;
                lights += lights_per_channel[i];
                m_lights_end[i] = lights;
                
                SPIConfigs[i] = new SPI.Configuration(pins[i], 
                                                    false,              //Chip Select Active State
                                                    0,                  //Chip Select Setup Time
                                                    0,                  //Chip Select Hold Time
                                                    true,               //Clock Idle State
                                                    true,               //Clock Edge
                                                    500,                //Clock Rate (kHz)
                                                    SPI.SPI_module.SPI1);//SPI Module

                
            }
            
            SPIBus = new SPI(SPIConfigs[0]); // Init the SPI bus with the first channel
            
            m_totallights = lights;

            m_lights = new byte[lights][];
            for (int i = 0; i < lights; i++)
                m_lights[i] = new byte[3];
             
        }
        
        public void set_color(int index, byte r, byte g, byte b)
        {
            m_lights[index][0] = r;
            m_lights[index][1] = g;
            m_lights[index][2] = b;
        }

        public void show() // change to support multiple channels
        {
            for (int channel = 0; channel < m_num_channels; channel++)
            {
                SPIBus.Config = SPIConfigs[channel]; // switch to the channel
                byte[] buffer = new byte[(m_lights_per_channel[channel] * 3 * 5) + 1];
                int bufferIdx = 0;
                for (int light = m_lights_start[channel]; light < m_lights_end[channel]; light++)
                {
                    for (int bit = 0; bit < 3; bit++) // r, g, b
                    {
                        for (int pos = 0; pos < 5; pos++) // each of the five bytes
                        {
                            byte toSend = lookup_byte(5, m_lights[light][bit]);
                            buffer[bufferIdx++] = toSend;
                        }
                    }
                }
                buffer[buffer.Length] = 0x00;
                SPIBus.Write(buffer); // send data and reset
            }
        }

        // five bytes (from MSB to LSB), four possibilities per byte (00 to 11)
        static byte[][] lookup = new byte[][]
        { 
            new byte[] {   0x84,  // _000__00  00xxxxxx 8th & 7th bits
                           0x87,  // _000__11  01xxxxxx
                           0xF4,  // _111__00  10xxxxxx
                           0xF7}, // _111__11  11xxxxxx
            new byte[] {   0x21,  // 0__000__  x00xxxxx 7th & 6th bits
                           0x3D,  // 0__111__  x01xxxxx
                           0xA1,  // 1__000__  x10xxxxx 
                           0xBD}, // 1__111__  x11xxxxx
            new byte[] {   0x08,  // 000__000  xxx00xxx 5th & 4th bits
                           0x0F,  // 000__111  xxx01xxx
                           0xE8,  // 111__000  xxx10xxx 
                           0xEF}, // 111__111  xxx11xxx
            new byte[] {   0x42,  // __000__0  xxxxx00x 3rd & 2nd bits
                           0x43,  // __000__1  xxxxx01x
                           0x7A,  // __111__0  xxxxx10x 
                           0x7B}, // __111__1  xxxxx11x
            new byte[] {   0x10,  // 00__000_  xxxxxx00 2nd & 1st bits
                           0x1E,  // 00__111_  xxxxxx01
                           0xD0,  // 11__000_  xxxxxx10 
                           0xD3}};// 11__111_  xxxxxx11
        static byte[] shift = { 0, 1, 3, 5, 6 };
        const byte mask = 0x03; // 0x00000011
        byte lookup_byte(int pos, byte data)
        {
            return lookup[pos][((data >> shift[pos]) & mask)];
        }
        
        
    }
}
