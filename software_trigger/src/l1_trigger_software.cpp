
/*
L1 trigger IP August 2017
Francesca Capel
capel.francesca@gmail.com


NB: SOFTWARE VERSION FOR VERIFICATION OF THE TESTBENCH IN VIVADO

L1 Trigger IP for the Mini-EUSO instrument, implemented as part of the Zynq board firmware
Test version for checking trigger response in software 
Only one trigger per 128 packets
Key parameters can be changed in the header file "L1_trigger_software.h"

An extra layer to the trigger logic has been added to make it robust to direct CR events
The implementationof this is currently being tested

The pixel mask is not yet implemented
*/

#include "l1_trigger_software.h"

int l1_trigger(char * FILE_NAME, std::ifstream& in_stream, std::ofstream& out_stream, uint16_t n_pixels_in_bus, int trig_data);

/* Pass input file streams instead of axi streams and adapt for not packed AXI STREAMS */
int l1_trigger(char * FILE_NAME, std::ifstream& in_stream, std::ofstream& out_stream, uint16_t n_pixels_in_bus, int trig_data){

  uint32_t l1_data[N_PIXELS];
  int skip_data[N_PIXELS];
  int trig_count = 0;

  char trig_file_name[40];
  strcpy(trig_file_name, FILE_PATH);
  strcat(trig_file_name, FILE_NAME);
  strcat(trig_file_name, ".tr1");
  
  std::cout << trig_file_name << std::endl;
  std::ofstream trig_file(trig_file_name, std::ios::out);

  //k, kk and kj for GTU index, i for pixel index
  int i, k, kk, itrig;
  int pkt_num = 0; 
  uint32_t sum_pixP;
  uint32_t sum_overP[N_PIXELS];
  uint32_t sum_pix[N_PIXELS], data_shift[P][N_PIXELS], thresh[N_PIXELS];
  
  //Initialisation
  trig_data = 0;
  for(i = 0; i < n_pixels_in_bus; i++) {
    sum_pix[i] = 0;
    l1_data[i] = 0;
    thresh[i] = 255;
  }
  
  for(kk = 0; kk < P; kk++) {
    for(i = 0; i < n_pixels_in_bus; i++) {
      data_shift[kk][i] = 0;
    }
  }

  //Go through the whole file
  while(!in_stream.eof()) {

    std::cout << std::endl << pkt_num << std::endl;
    //1 trigger per 128 packets
    itrig = 0;

    for(i = 0; i < n_pixels_in_bus; i++) {
      sum_pix[i] = 0;
    }
    
    //Read data in and loop over 128 packets performing triggering  
    for(k = 0; k < N_ADDS; k++) {
        
      //Loop over 1 packet
      for(i = 0; i < n_pixels_in_bus; i++) {
	
	//Read in the data, ignoring the GTU number labels
	if (i == 0 || i % 2305 == 0) {
	  in_stream >> skip_data[i];
	  in_stream >> l1_data[i];
	}
	else {
	  in_stream >> l1_data[i];
	}
	//std::cout << l1_data[i] << ' '; 
 
	//Make sum
	sum_pix[i] += l1_data[i];
	
	//Make moving average over P GTU
	sum_overP[i] = 0;
	for (kk = P - 2; kk >= 0; kk--) {
	  data_shift[kk + 1][i] = data_shift[kk][i];
	  sum_overP[i] += data_shift[kk+1][i];
	}
	
	data_shift[0][i] = l1_data[i];
	sum_overP[i] += data_shift[0][i];
	
	//If rise is instantaneous, block for rest of 128 GTU (CR block)
	uint32_t check = 0; 
	
	//Ignore negative check values
	if (l1_data[i] < data_shift[1][i]) {
	  check = 0;
	}
	else {
	  check = (l1_data[i] - data_shift[1][i]);
	}

	//DH Block
	if (check > RISE_THRESH) {
	  itrig = 1;
	}

	//Trigger decision
	//If moving average is above threshold
       	if(sum_overP[i] > thresh[i]) {
	  
	  //No previous trigger in this packet of 128 GTU
	  if(itrig == 0) {
	    //Pulse trigger wire for 1 clock (only for hardware implementation)
	    trig_data = 0x00000001;
	    trig_data = 0x00000000;
	    //Write to the .tr1 file
	    trig_file << k << ' ' << pkt_num << ' ' << i << ' '
		      << l1_data[i] << ' ' << l1_data[i-1] << ' '<< l1_data[i+1] << std::endl;
	    itrig = 1;
	    trig_count++;
	  } 
	}
      }
    }
    
    
    //Write to the output stream and set the threshold
    for(i = 0; i < n_pixels_in_bus; i++) {
      
      out_stream << sum_pix[i] << ' ';
      
      sum_pixP = (P * sum_pix[i]) / N_ADDS;
      thresh[i] = sum_pixP + (N_SIGMA * sqrt(sum_pixP));
      //thresh[i] = N*sum_pixP;
      // std::cout << thresh[i] << ' ';
 
      
      if (thresh[i] < LOW_THRESH) {
	thresh[i] = LOW_THRESH;
      }

    }
    
    /* go to the next packet */
    pkt_num++;
  }

  std::cout << "GTU # " << k << std::endl;
  std::cout << "Packet # " << pkt_num << std::endl;
  
  return trig_count;
}
  
int main(int argc, char *argv[]) {
  
  char * FILE_NAME = argv[1];
  /* define data files */
  char in_file_name[40];
  strcpy(in_file_name, FILE_PATH);
  strcat(in_file_name, FILE_NAME);
  strcat(in_file_name, ".dl1");

  char out_file_name[40];
  strcpy(out_file_name, FILE_PATH);
  strcat(out_file_name, FILE_NAME);
  strcat(out_file_name, ".dl2");

  // Print the file names
  std::cout << in_file_name << std::endl;
  std::cout << out_file_name << std::endl;
  
  std::ifstream in_stream(in_file_name, std::ios::in);
  std::ofstream out_stream(out_file_name, std::ios::out);

  
  /* define inputs */
  uint16_t n_pixels_in_bus = 2304;
  int trig_data = 0;
  int trig_count = 0;
  
  trig_count = l1_trigger(FILE_NAME, in_stream, out_stream, n_pixels_in_bus, trig_data);
  
  in_stream.close();
  out_stream.close();
  
  std::cout << "Number of triggers: " << trig_count << std::endl;
  
  return 0;
}

