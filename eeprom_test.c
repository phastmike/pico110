#include "m110.h"
#include <stdio.h>

unsigned char ram[128] = {
   0x52, 0x53, 0x41, 0x31, 0x35, 0x32, 0x35, 0x45, 
   0x5a, 0x41, 0xdf, 0x02, 0x00, 0x00, 0x00, 0x67,
   0x58, 0x00, 0x50, 0x16, 0x81, 0x12, 0x01, 0x02,
   0x21, 0x02, 0x20, 0x02, 0x7c, 0xe6, 0x41, 0xa2,
   0x00, 0x00, 0x4e, 0x32, 0x6c, 0x02, 0x7c, 0xe6, 
   0x41, 0xa2, 0x13, 0x06, 0x4e, 0x32, 0x7c, 0x00,
};

void main(void) {
   printf("- Eeprom init...\n");
   eeprom_t *eeprom = eeprom_new_with_data(ram);

   printf("\taddr = %d\n", eeprom_addr_get(eeprom));

   printf("\tMem:\n\n");

   printf("Sequential Read\n");
   for(int i=0; i < 128; i++) {
      printf("0x%02x ", eeprom_read_byte(eeprom));
      if ((i+1) % 8 == 0) printf("\n");
   }
   printf("\n");


   printf("Sequential Read but setting addr before\n");
   for(int i=0; i < 128; i++) {
      eeprom_addr_set(eeprom, (unsigned int) i);
      printf("0x%02x ", eeprom_read_byte(eeprom));
      if ((i+1) % 8 == 0) printf("\n");
   }
   printf("\n");

   printf("Sequential write\n");
   for(int i=0; i < 128; i++) {
      eeprom_addr_set(eeprom, (unsigned int) i);
      eeprom_write_byte(eeprom, 0xff);
   }
   printf("\n");

   printf("Sequential Read\n");
   eeprom_addr_set(eeprom, (unsigned int) 0); // redundant
   for(int i=0; i < 128; i++) {
      printf("0x%02x ", eeprom_read_byte(eeprom));
      if ((i+1) % 8 == 0) printf("\n");
   }
   printf("\n");


   m110_t *m110 = m110_new_with_data(ram);
   m110_channel_frequencies_set(m110, 1, 467.275, 467.275);
   m110_channel_frequencies_set(m110, 2, 467.400, 467.400);
   //m110_eeprom_set_checksum(m110, 0x5b);

   printf("Sequential Read\n");
   eeprom = m110_eeprom_get(m110);
   eeprom_addr_set(eeprom, (unsigned int) 0); // redundant
   for(int i=0; i < 128; i++) {
      printf("0x%02x ", eeprom_read_byte(eeprom));
      if ((i+1) % 8 == 0) printf("\n");
   }
   printf("\n");

   // to file
   unsigned char value;
   FILE *fp = fopen("m110.bin", "w+");
   eeprom_addr_set(eeprom, (unsigned int) 0); // redundant
   for(int i=0; i < 128; i++) {
      value = eeprom_read_byte(eeprom);
      fwrite(&value, 1, 1, fp);
   }
   fclose(fp);

}
